#include <simlib.h>

const int POC_POKLADEN = 5;

Facility Pokladny[POC_POKLADEN];
Store Lahudky("Oddělení lahůdek", 2);
Store Voziky("Seřadiště vozíků", 50);

Histogram celk("Celková doba v systému", 0, 5, 20);

class Zakaznik : public Process
{
    void Behavior()
    {
        double prichod = Time;
        Enter(Voziky, 1);
        if (Random() < 0.3)
        {
            Enter(Lahudky, 1);
            Wait(Exponential(2));
            Leave(Lahudky, 1);
        }
        Wait(Uniform(10, 15));

        int i = 0;
        for (int a = 1; a < POC_POKLADEN; a++)
        {
            if (Pokladny[a].QueueLen() < Pokladny[i].QueueLen())
                i = a;
        }
        Seize(Pokladny[i]);
        Wait(Exponential(3));
        Release(Pokladny[i]);
        Leave(Voziky, 1);
        celk(Time - prichod);
    }
};

class Prichody : public Event
{
    void Behavior()
    {
        (new Zakaznik)->Activate();
        Activate(Time + Exponential(8));
    }
};

int main()
{
    SetOutput("samoo.dat");
    Init(0, 1000);
    (new Prichody)->Activate();
    Run();

    celk.Output();
    Lahudky.Output();
    Voziky.Output();
    for (int a = 0; a < POC_POKLADEN; a++)
        Pokladny[a].Output();
    return 0;
}
