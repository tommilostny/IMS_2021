#include <iostream>
#include <simlib.h>
#include <vector>

#define YEARS 10
#define HOURS_IN_YEAR 8760
#define HOURS_IN_DAY 24
#define DAYS_IN_YEAR 365

class Storage
{
private:
    uint64_t storedChips = 0;    // number of chips stored
    uint64_t awaitingOrders = 0; // amount of ordered chips awaiting to be processed
public:
    void Add(uint64_t chips)
    {     
        if (awaitingOrders > 0)
        {
            if (chips > awaitingOrders)
            {
                storedChips += chips - awaitingOrders;
                awaitingOrders = 0;
            }
            else
            {
                awaitingOrders -= chips;
            }
        }
        else
        {
            storedChips += chips;
        }
        std::cout << "Stored: " << storedChips << ", awaiting: " << awaitingOrders << " chips" << std::endl;
    }

    void Retrieve(uint64_t chips)
    {
        if (storedChips >= chips)
        {
            storedChips -= chips;
        }
        else
        {
            awaitingOrders += chips - storedChips;
            storedChips = 0;
        }
    }
};

class Producer : public Event
{
private:
    uint64_t hourlyChipProduction;
    std::string name;
    uint64_t totalProduced = 0;
    Storage* globalStorage; // global storage to contribute produced chips to
public:
    Producer(uint64_t yearlyWaferProduction, uint16_t chipsPerWafer, std::string name, Storage* global) : name(name), globalStorage(global)
    {
        hourlyChipProduction = yearlyWaferProduction * chipsPerWafer / HOURS_IN_YEAR;
    }

    void Behavior()
    {
        auto produced = hourlyChipProduction * Normal(1.0, 0.1);
        
        totalProduced += produced;
        std::cout << name << ": total " << totalProduced << ", adding " << produced << " chips" << std::endl;

        globalStorage->Add(produced);
        Activate(Time + 1);
    }

    void UpdateProduction(double rate)
    {
        hourlyChipProduction *= rate;
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

class YearTracker : public Event
{
public:
    uint16_t year;
    std::vector<Producer*> producers;
    std::vector<Consumer*> consumers;

    YearTracker(uint16_t startYear, std::vector<Producer*> producers, std::vector<Consumer*> consumers)
        : year(startYear), producers(producers), consumers(consumers)
    {
    }

    void Behavior()
    {
        std::cout << year << " -> ";
        year++;
        std::cout << year << std::endl;

        for (auto consumer : consumers)
            consumer->AddToOrderRate(0.1);

        Activate(Time + HOURS_IN_YEAR);
    }
};

int main()
{
    // Simulate from 2020 to 2030 with 1 hour time step
    Init(0, HOURS_IN_YEAR * YEARS);
    auto globalStorage = new Storage();

    std::vector<Producer*> producers = 
    {
        new Producer(12500000, 600, "TSMC", globalStorage),
        new Producer(3060000 * 12, 300, "Samsung", globalStorage)
        //auto Intel
        //auto others
    };

    std::vector<Consumer*> consumers = 
    {
        new Consumer(15000000000, "Automotive", globalStorage)
    };

    for (auto producer : producers)
        producer->Activate();
    
    for (auto consumer : consumers)
        consumer->Activate(Time + HOURS_IN_DAY);

    // Create year tracker that updates yearly production and order rates
    (new YearTracker(2020, producers, consumers))->Activate(Time + HOURS_IN_YEAR);

    Run();

    delete globalStorage;
    return 0;
}
