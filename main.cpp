#include <iostream>
#include <simlib.h>
#include <vector>

#define SIMULATION_YEARS 7

#define HOURS_IN_MONTH 730
#define HOURS_IN_YEAR 8760
#define HOURS_IN_DAY 24
#define DAYS_IN_YEAR 365

#define NEW_FACTORY_CHIPS_YEARLY (43792982460)
#define NEW_FACTORY_CHIPS_HOURLY (NEW_FACTORY_CHIPS_YEARLY / HOURS_IN_YEAR)

class Storage
{
private:
    uint64_t storedChips = 0;    // number of chips stored
    uint64_t awaitingOrders = 0; // amount of ordered chips awaiting to be processed
public:
    Storage(uint64_t startStorage) : storedChips(startStorage) {}

    void Add(uint64_t chips)
    {     
        if (awaitingOrders > 0)
        {
            if (chips > awaitingOrders)
            {
                storedChips += chips - awaitingOrders;
                awaitingOrders = 0;
            }
            else awaitingOrders -= chips;
        }
        else storedChips += chips;

        std::cout << "Stored: " << storedChips << ", awaiting: " << awaitingOrders << " chips" << std::endl;
    }

    void Retrieve(uint64_t chips)
    {
        if (storedChips < chips)
        {
            awaitingOrders += chips - storedChips;
            storedChips = 0;
            return;
        }
        storedChips -= chips;
    }
};

class Producer : public Event
{
private:
    uint64_t hourlyChipProduction;
    std::string name;
    uint64_t totalProduced = 0;
    Storage* globalStorage; // global storage to contribute produced chips to

    std::vector<uint16_t> newFactoriesMonths; // months when new factories are created
    std::vector<uint16_t> newFactoriesYears;  // years when new factories are created
public:
    Producer(uint64_t yearlyWaferProduction, uint16_t chipsPerWafer, std::string name, Storage* global, std::vector<uint16_t> newFactoriesMonths, std::vector<uint16_t> newFactoriesYears)
        : name(name), globalStorage(global), newFactoriesMonths(newFactoriesMonths), newFactoriesYears(newFactoriesYears)
    {
        hourlyChipProduction = yearlyWaferProduction * chipsPerWafer / HOURS_IN_YEAR;
    }

    Producer(uint64_t yearlyChipProduction, std::string name, Storage* global, std::vector<uint16_t> newFactoriesMonths, std::vector<uint16_t> newFactoriesYears)
        : name(name), globalStorage(global), newFactoriesMonths(newFactoriesMonths), newFactoriesYears(newFactoriesYears)
    {
        hourlyChipProduction = yearlyChipProduction / HOURS_IN_YEAR;
    }

    void Behavior()
    {
        auto produced = hourlyChipProduction * Normal(1.0, 0.01);
        
        totalProduced += produced;
        //std::cout << name << ": total " << totalProduced << ", adding " << produced << " chips" << std::endl;

        globalStorage->Add(produced);
        Activate(Time + 1);
    }

    void UpdateProduction(uint16_t month, uint16_t year)
    {
        for (int i = 0; i < newFactoriesMonths.size(); i++)
        {
            if (newFactoriesMonths[i] == month && newFactoriesYears[i] == year)
            {
                std::cout << name << ": new factory created in " << month << "/" << year << ", increasing production from " << hourlyChipProduction;

                hourlyChipProduction *= 1.4;//NEW_FACTORY_CHIPS_HOURLY;
                
                std::cout << " to " << hourlyChipProduction << std::endl;
            }
        }
        //std::cout << name << ": " << month << "/" << year << std::endl;
    }
};

class Consumer : public Event
{
private:
    uint64_t dailyChipConsumption;
    double orderRate = 1.0;
    std::string name;
    Storage* globalStorage; // global storage to order chips from
public:
    Consumer(uint64_t yearlyChipsConsumption, std::string name, Storage* global) : globalStorage(global), name(name)
    {
        dailyChipConsumption = yearlyChipsConsumption / DAYS_IN_YEAR;
    }

    void Behavior()
    {
        auto chipsToOrder = dailyChipConsumption * orderRate;
        globalStorage->Retrieve(chipsToOrder);
        Activate(Time + HOURS_IN_DAY);
    }

    void AddToOrderRate(double rate)
    {
        orderRate += rate;
    }
};

class MonthYearTracker : public Event
{
public:
    uint16_t year;
    uint16_t month = 1;
    std::vector<Producer*> producers;
    std::vector<Consumer*> consumers;

    MonthYearTracker(uint16_t startYear, std::vector<Producer*> producers, std::vector<Consumer*> consumers)
        : year(startYear), producers(producers), consumers(consumers)
    {
    }

    void Behavior()
    {
        if (++month > 12)
        {
            month = 1;
            year++;

            for (auto consumer : consumers)
                consumer->AddToOrderRate(Normal(0.1, 0.001));
        }
        for (auto producer : producers)
            producer->UpdateProduction(month, year);

        Activate(Time + HOURS_IN_MONTH);
    }
};

int main()
{
    // Simulate from 2020 to 2027 with 1 hour time step
    Init(0, HOURS_IN_YEAR * SIMULATION_YEARS);
    auto globalStorage = new Storage(1000000);

    std::vector<uint16_t> tsmcNewFactoryMonths = { 10, 1 };
    std::vector<uint16_t> tsmcNewFactoryYears = { 2021, 2024 };

    std::vector<uint16_t> samsungNewFactoryMonths = { 6 };
    std::vector<uint16_t> samsungNewFactoryYears = { 2024 };

    std::vector<uint16_t> umcNewFactoryMonths = { 4 };
    std::vector<uint16_t> umcNewFactoryYears = { 2023 };

    std::vector<uint16_t> smicNewFactoryMonths = { 1, 1 };
    std::vector<uint16_t> smicNewFactoryYears = { 2022, 2024 };

    std::vector<uint16_t> intelNewFactoryMonths = { 10, (uint16_t)(4 + Uniform(6, 9)), 6, 6 };
    std::vector<uint16_t> intelNewFactoryYears = { 2021, 2021, 2024, 2024 };

    std::vector<Producer*> producers = 
    {
        new Producer(13000000, 7000, "TSMC", globalStorage, tsmcNewFactoryMonths, tsmcNewFactoryYears),
        new Producer(3060000 * 12, 3500, "Samsung", globalStorage, samsungNewFactoryMonths, samsungNewFactoryYears),
        new Producer(800000 * 12, 3500, "UMC", globalStorage, umcNewFactoryMonths, umcNewFactoryYears),
        new Producer(120000 * 12, 7000, "SMIC", globalStorage, smicNewFactoryMonths, smicNewFactoryYears),
        new Producer(159170967742, "Others", globalStorage, {}, {}),
        new Producer(240842105263, "Intel", globalStorage, intelNewFactoryMonths, intelNewFactoryYears)
    };

    std::vector<Consumer*> consumers = 
    {
        new Consumer(88000000000, "Automotive", globalStorage),
        new Consumer(240842105263, "Communications", globalStorage),
        new Consumer(249333333333, "Computers", globalStorage),
        new Consumer(92631578947, "Consumer electronics", globalStorage),
        new Consumer(92631578947, "Industrial", globalStorage),
        new Consumer(7719298246, "Government", globalStorage),
    };

    for (auto producer : producers)
        producer->Activate();
    
    for (auto consumer : consumers)
        consumer->Activate(Time + HOURS_IN_DAY);

    // Create year tracker that updates yearly production and order rates
    (new MonthYearTracker(2020, producers, consumers))->Activate(Time + HOURS_IN_MONTH);

    Run();

    delete globalStorage;
    return 0;
}
