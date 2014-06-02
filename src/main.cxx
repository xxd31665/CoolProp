

#include "Backends/Helmholtz/HelmholtzEOSBackend.h"
#include "Backends/Helmholtz/HelmholtzEOSMixtureBackend.h"
#include "Backends/REFPROP/REFPROPMixtureBackend.h"
#include "Backends/REFPROP/REFPROPBackend.h"
#include <time.h>
#include "AbstractState.h"
#include "DataStructures.h"
#include <cstdio>
#include "CoolProp.h"
using namespace CoolProp;

#include "rapidjson/rapidjson_include.h"
#include "Backends/Helmholtz/Fluids/FluidLibrary.h"
#ifdef ENABLE_CATCH
#include "Tests.h"
#endif
#include "SpeedTest.h"

//#include <vld.h>

void generate_melting_curve_data(const char* file_name, const char *fluid_name, double Tmin, double Tmax)
{

    FILE *fp;
    fp = fopen(file_name,"w");
    std::tr1::shared_ptr<AbstractState> State(AbstractState::factory(std::string("REFPROP"),std::string(fluid_name)));
    for (double T = Tmin; T < Tmax; T += 0.1)
    {
        try{
            double pp = State->calc_melt_p_T(T);
            State->update(PT_INPUTS,pp,T);
            double rho = State->rhomolar();
            State->update(DmolarT_INPUTS,rho,T);
            double pp2 = State->p();
            if  (fabs(pp2-pp) > 0.01)
            {
                double rr = 0;
            }
            //printf("%g,%g,%g\n",T,pp,rho);
            fprintf(fp, "%g,%g,%g\n",T,pp,rho);
        }
        catch(std::exception &e)
        {

            std::cout << fluid_name << " " << e.what() << std::endl;
            break;
        }
    }
    fclose(fp);
}
struct element
        {
            double d,t,ld;
            int l;
        };
int main()
{
    set_debug_level(0);
    if (1)
    {
        std::string NBP_refs[] = {"Helium","Ethylene","Ethanol","n-Dodecane","Benzene","n-Undecane","Neon","Fluorine","Methanol","Acetone","Methane","Ethane","n-Pentane","n-Hexane","n-Heptane","n-Octane","CycloHexane","MD3M","MM","D4","MethylPalmitate","MethylStearate","MethylOleate","MethylLinoleate","MethylLinolenate","m-Xylene","Air"};
        std::string IIR_refs[] = {"SES36","R143a","CycloPropane","Propylene","R227EA","R365MFC","R161","HFE143m","SulfurHexafluoride","CarbonDioxide","R1234ze(E)","R22","R124","Propyne","R507A","R152A","R123","R11","n-Butane","IsoButane","RC318","R21","R114","R13","R12","R113","R1233zd(E)","R41"};
        for (std::size_t i = 0; i < sizeof(NBP_refs)/sizeof(NBP_refs[0]); ++i)
        {
            try{
                //set_reference_stateS(NBP_refs[i],"RESET");
                HelmholtzEOSMixtureBackend HEOS(std::vector<std::string>(1,NBP_refs[i]));
                HEOS.update(PQ_INPUTS, 101325, 0);
                double delta_a1 = HEOS.smass()/(HEOS.gas_constant()/HEOS.molar_mass());
                double delta_a2 = -HEOS.hmass()/(HEOS.gas_constant()/HEOS.molar_mass()*HEOS.get_reducing().T);
                std::cout << format("%s,%s,%16.15g,%16.15g\n",NBP_refs[i].c_str(),"NBP",delta_a1, delta_a2);
            }
            catch(const std::exception &e)
            {
                std::cout << "ERROR FOR " << NBP_refs[i] << std::endl;
            }
        }
        for (std::size_t i = 0; i < sizeof(IIR_refs)/sizeof(IIR_refs[0]); ++i)
        {
            try{
                //set_reference_stateS(IIR_refs[i],"RESET");
                HelmholtzEOSMixtureBackend HEOS(std::vector<std::string>(1,IIR_refs[i]));
                HEOS.update(QT_INPUTS, 0, 273.15);
                double delta_a1 = (HEOS.smass()-1000)/(HEOS.gas_constant()/HEOS.molar_mass());
                double delta_a2 = -(HEOS.hmass()-200000)/(HEOS.gas_constant()/HEOS.molar_mass()*HEOS.get_reducing().T);
                std::cout << format("%s,%s,%16.15g,%16.15g\n",IIR_refs[i].c_str(),"IIR",delta_a1, delta_a2);
            }
            catch(const std::exception &e)
            {
                std::cout << "ERROR FOR " << IIR_refs[i] << std::endl;
            }
        }
        double rr = 0;
    }
    if (0)
    {
        generate_melting_curve_data("Ethylene-I.mlt","ethylene",103.989,110.369);
        generate_melting_curve_data("Ethylene-II.mlt","ethylene",110.369,450);
        generate_melting_curve_data("Propylene-I.mlt","propylen",87.953,109.6);
        generate_melting_curve_data("Propylene-II.mlt","propylen",109.6,575);
        generate_melting_curve_data("ParaHydrogen-I.mlt","parahyd",13.8033,22);
        generate_melting_curve_data("ParaHydrogen-II.mlt","parahyd",22,2000);

        generate_melting_curve_data("n-Propane.mlt","propane",85.53,2000);
        generate_melting_curve_data("n-Butane.mlt","butane",134.9,2000);
        generate_melting_curve_data("n-Pentane.mlt","pentane",143.5,2000);
        generate_melting_curve_data("IsoButane.mlt","isobutan",113.73,2000);
        generate_melting_curve_data("Isopentane.mlt","ipentane",112.66,2000);
        generate_melting_curve_data("Argon.mlt","argon",83.8058,2000);
        generate_melting_curve_data("Ethane.mlt","ethane",90.37,2000);
        generate_melting_curve_data("Nitrogen.mlt","nitrogen",63.151,2000);
        generate_melting_curve_data("Fluorine.mlt","fluorine",53.4811,2000);
        generate_melting_curve_data("Methane.mlt","methane",90.70,2000);
        generate_melting_curve_data("Methanol.mlt","methanol",175.61,2000);
        generate_melting_curve_data("Krypton.mlt","krypton",115.775,2000);
        generate_melting_curve_data("Xenon.mlt","xenon",161.405,2000);
        generate_melting_curve_data("CarbonMonoxide.mlt","co",68.16,2000);
        generate_melting_curve_data("Oxygen.mlt","oxygen",54.361,2000);
        generate_melting_curve_data("CycloHexane.mlt","cyclohex",279.7,2000);
        generate_melting_curve_data("CarbonDioxide.mlt","CO2",217,2000);
    }
    if (0)
    {
        std::cout << "Water DmolarT at 1e-3,300 \n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("Water",DmolarT_INPUTS, 1e-3, 300, 10000, 0, 1e-8);
        std::cout << "Water PT at 101325,300 \n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("Water",PT_INPUTS, 101325, 300, 10000, 0, 1e-8);
        std::cout << "Water QT at 350 K\n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("Water",QT_INPUTS, 1, 350, 10000, 0, 1e-8);
        std::cout << "Water PQ at 101325 Pa\n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("Water",PQ_INPUTS, 101325, 1, 10000, 1e-2, 0);

        std::cout << "R134a DmolarT at 1e-3,300 \n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("R134a",DmolarT_INPUTS, 1e-3, 300, 10000, 0, 1e-8);
        std::cout << "R134a PT at 101325,300 \n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("R134a",PT_INPUTS, 101325, 300, 10000, 0, 1e-8);
        std::cout << "R134a QT at 350 K\n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("R134a",QT_INPUTS, 1, 350, 10000, 0, 1e-8);
        std::cout << "R134a PQ at 101325 Pa\n-----------------\n";
        CoolProp::compare_REFPROP_and_CoolProp("R134a",PQ_INPUTS, 101325, 1, 10000, 1e-2, 0);
    }
    if (0)
    {
        std::vector<std::string> ss = strsplit(get_global_param_string("FluidsList"),',');

        for (std::vector<std::string>::iterator it = ss.begin(); it != ss.end(); ++it)
        {
            AbstractState *S = AbstractState::factory("HEOS", (*it));
            S->update(QT_INPUTS, 0, S->Ttriple());
            std::cout << format("%s %17.15g\n", S->name().c_str(), S->p());
        }
    }
    if (1)
    {
        double rrr0 = PropsSI("O","T",350,"Q",0,"REFPROP::Water");
        double rrr2 = PropsSI("O","T",350,"Q",0,"Water");
        double rrr =0 ;
    }
    if (1)
    {
        std::tr1::shared_ptr<AbstractState> ASR(AbstractState::factory("REFPROP","CO2"));
        ASR->update(QT_INPUTS, 1, 304);
        double muR0 = ASR->conductivity();

        std::tr1::shared_ptr<AbstractState> ASC(AbstractState::factory("HEOS","CO2"));
        ASC->update(QT_INPUTS, 1, 304);
        double muC = ASC->conductivity();
        double rr = 4;
    }
    if (1)
    {
        double h1 = PropsSI("H","T",273.15,"Q",0,"CO2");
        set_reference_stateS("n-Propane","NBP");
        double h2 = PropsSI("H","P",101325,"Q",0,"n-Propane");

        std::string RPname = get_fluid_param_string("Water", "REFPROPname");
        std::string s = get_BibTeXKey("n-Propane", "rr");

        std::vector<std::string> tags;
        tags.push_back("[helmholtz]");
        run_user_defined_tests(tags);
        run_tests();

        std::string fl = get_global_param_string("FluidsList");
        double rr = PropsSI("D", "P", 3e5, "T", 300, "Nitrogen");
        std::tr1::shared_ptr<AbstractState> AS(AbstractState::factory("HEOS","Nitrogen"));

        AS->update(DmolarT_INPUTS, 40, 300);
        double p1 = AS->umolar();
        double d1 = AS->rhomolar();
        double T1 = AS->delta();
        double dpdT_constrho = AS->first_partial_deriv(iUmolar, iDelta, iTau);
        AS->update(DmolarT_INPUTS, 40+1e-6, 300);
        double p2 = AS->umolar();
        double d2 = AS->rhomolar();
        double T2 = AS->delta();

        double dpdT_constrho2 = (p2-p1)/(T2-T1);

        AS->update(PT_INPUTS, 101000, 300);

        std::vector<double> T(2,300), P(2,101325), o, z(1,1);
        std::string in1 = "Dmass", in2 = "T", in3 = "P", Ref = "Nitrogen";
        T[1] = 400;
        o = PropsSI(in1,in2,T,in3,P,Ref,z);
        double tr = 0;
    }
    if (0)
    {
        // First type (slowest, most string processing, exposed in DLL)
        double r0A = PropsSI("Dmolar","T",298,"P",1e5,"Propane[0.5]&Ethane[0.5]"); // Default backend is HEOS
        double r0B = PropsSI("Dmolar","T",298,"P",1e5,"HEOS::Propane[0.5]&Ethane[0.5]");
        double r0C = PropsSI("Dmolar","T",298,"P",1e5,"REFPROP::Propane[0.5]&Ethane[0.5]");

        std::vector<double> z(2,0.5);
        // Second type (C++ only, a bit faster)
        double r1A = PropsSI("Dmolar","T",298,"P",1e5,"Propane&Ethane", z);
        double r1B = PropsSI("Dmolar","T",298,"P",1e5,"HEOS::Propane&Ethane", z);
        double r1C = PropsSI("Dmolar","T",298,"P",1e5,"REFPROP::Propane&Ethane", z);

        //const double *pz = &(z[0]);
        //int n = z.size();
        //// Third type (DLL)
        //double r2A = PropsSIZ("Dmolar","T",298,"P",1e5,"Propane&Ethane", pz, n);
        //double r2B = PropsSIZ("Dmolar","T",298,"P",1e5,"HEOS::Propane&Ethane", pz, n);
        //double r2C = PropsSIZ("Dmolar","T",298,"P",1e5,"REFPROP::Propane&Ethane", pz, n);

        double tt = 0;
    }

    if (0)
    {
        #ifdef ENABLE_CATCH
        run_tests();
        #endif
    }
    if (0)
    {

        double n[] = {0.0125335479355233,                        7.8957634722828,                        -8.7803203303561,                        0.31802509345418,                        -0.26145533859358,                        -0.0078199751687981,                        0.0088089493102134,                        -0.66856572307965,                        0.20433810950965,                        -6.621260503968699e-005,                        -0.19232721156002,                        -0.25709043003438,                        0.16074868486251,                        -0.040092828925807,                        3.9343422603254e-007,                        -7.5941377088144e-006,                        0.00056250979351888,                        -1.5608652257135e-005,                        1.1537996422951e-009,                        3.6582165144204e-007,                        -1.3251180074668e-012,                        -6.2639586912454e-010,                        -0.10793600908932,                        0.017611491008752,                        0.22132295167546,                        -0.40247669763528,                        0.58083399985759,                        0.0049969146990806,                        -0.031358700712549,                        -0.74315929710341,                        0.4780732991548,                        0.020527940895948,                        -0.13636435110343,                        0.014180634400617,                        0.008332650488071301,                        -0.029052336009585,                        0.038615085574206,                        -0.020393486513704,                        -0.0016554050063734,                        0.0019955571979541,                        0.00015870308324157,                        -1.638856834253e-005,                        0.043613615723811,                        0.034994005463765,                        -0.076788197844621,                        0.022446277332006,                        -6.2689710414685e-005,                        -5.5711118565645e-010,                        -0.19905718354408,                        0.31777497330738,                        -0.11841182425981  };
        double d[] = {1,                         1,                        1,                        2,                        2,                        3,                        4,                        1,                        1,                        1,                        2,                        2,                        3,                        4,                        4,                        5,                        7,                        9,                        10,                        11,                        13,                        15,                        1,                        2,                        2,                        2,                        3,                        4,                        4,                        4,                        5,                        6,                        6,                        7,                        9,                        9,                        9,                        9,                        9,                        10,                        10,                        12,                        3,                        4,                        4,                        5,                        14,                        3,                        6,                        6,                        6                    };
        double t[] = {-0.5,                        0.875,                        1,                        0.5,                        0.75,                        0.375,                        1,                        4,                        6,                        12,                        1,                        5,                        4,                        2,                        13,                        9,                        3,                        4,                        11,                        4,                        13,                        1,                        7,                        1,                        9,                        10,                        10,                        3,                        7,                        10,                        10,                        6,                        10,                        10,                        1,                        2,                        3,                        4,                        8,                        6,                        9,                        8,                        16,                        22,                        23,                        23,                        10,                        50,                        44,                        46,                        50                    };
        double l[] = {0,                        0,                        0,                        0,                        0,                        0,                        0,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        1,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        2,                        3,                        3,                        3,                        3,                        4,                        6,                        6,                        6,                        6                    };
        double summer = 0;
        std::vector<element> elements;
        for (std::size_t i = 0; i < 51; ++i)
        {
            element el;
            el.d = d[i];
            el.t = t[i];
            el.l = (int)l[i];
            el.ld = (double)l[i];
            elements.push_back(el);
        }

        long N = 1000000;
        double t1 = clock();
        for (std::size_t ii = 0; ii < N; ++ii)
        {
            double delta = 1.3, tau = 0.7;
            double log_tau  = log(tau), log_delta = log(delta);
            for (int jj = 0; jj < 51; ++jj)
            {
                double di = d[jj], ti = t[jj], lid = l[jj];
                int li = (int)lid;
                double pow_delta_li;
                if (li > 0){
                    pow_delta_li = pow(delta, li);
                    summer += (di-lid*pow_delta_li)*exp(ti*log_tau+(di-1)*log_delta-pow_delta_li);
                }
                else{
                    summer += di*exp(ti*log_tau+(di-1)*log_delta);
                }
            }
        }
        double t2 = clock();
        double elap = (t2-t1)/CLOCKS_PER_SEC/((double)N)*1e6;
        printf("%g %g\n",elap, summer);

    }
    if (0)
    {
        std::tr1::shared_ptr<AbstractState> MixRP(AbstractState::factory(std::string("REFPROP"),std::string("propane")));
        MixRP->update(QT_INPUTS, 0, 330);
        long double s1 = MixRP->surface_tension();

        std::tr1::shared_ptr<AbstractState> Mix(AbstractState::factory(std::string("HEOS"), std::string("propane")));
        Mix->update(QT_INPUTS, 0, 330);
        long double s2 = Mix->surface_tension();
    }
    if (0)
    {

        double T = 300;

        std::tr1::shared_ptr<AbstractState> MixRP(AbstractState::factory(std::string("REFPROP"),std::string("propane")));
        {
            long N = 100000;
            double t1 = clock(), summer = 0;
            for (std::size_t ii = 0; ii < N; ++ii)
            {
                MixRP->update(QT_INPUTS, 0.0, T+10/((double)N)*ii);
                summer += MixRP->p();
            }
            double t2 = clock();
            double elap = (t2-t1)/CLOCKS_PER_SEC/((double)N)*1e6;
            printf("%g %g\n",elap, summer);
        }
        double p2 = MixRP->p();
        double cv2 = MixRP->cvmolar();
        double cp2 = MixRP->cpmolar();
        double T2 = MixRP->T();

        std::tr1::shared_ptr<AbstractState> Mix(AbstractState::factory(std::string("HEOS"), std::string("propane")));
        {
            long N = 100000;
            double t1 = clock(), summer = 0;
            for (std::size_t ii = 0; ii < N; ++ii)
            {
                Mix->update(QT_INPUTS, 0.0, T+10/((double)N)*ii);
                summer += Mix->p();
            }
            double t2 = clock();
            double elap = (t2-t1)/CLOCKS_PER_SEC/((double)N)*1e6;
            printf("%g %g\n",elap, summer);
        }
        double p1 = Mix->p();
        double cv1 = Mix->cvmolar();
        double cp1 = Mix->cpmolar();
        double T1 = Mix->T();

        double rr = 0;
    }
    if (0)
    {
        int N = 2;
        std::vector<long double> z(N, 1.0/N);
        double Q = 1, T = 250, p = 300000;

        int inputs = PQ_INPUTS; double val1 = p, val2 = Q;

        std::tr1::shared_ptr<AbstractState> MixRP(AbstractState::factory(std::string("REFPROP"), std::string("Ethane,propane")));
        MixRP->set_mole_fractions(z);
        MixRP->update(inputs, val1, val2);
        double p2 = MixRP->p();
        double rho2 = MixRP->rhomolar();
        double cv2 = MixRP->cvmolar();
        double cp2 = MixRP->cpmolar();
        double w2 = MixRP->speed_sound();
        double h2 = MixRP->hmolar();
        double s2 = MixRP->smolar();
        double phi20 = MixRP->fugacity_coefficient(0);
        double phi21 = MixRP->fugacity_coefficient(1);

        std::tr1::shared_ptr<AbstractState> Mix(AbstractState::factory(std::string("HEOS"), std::string("Ethane,propane")));
        Mix->set_mole_fractions(z);
        Mix->update(inputs, val1, val2);
        double p1 = Mix->p();
        double cv1 = Mix->cvmolar();
        double cp1 = Mix->cpmolar();
        double w1 = Mix->speed_sound();
        double h1 = Mix->hmolar();
        double s1 = Mix->smolar();
        double phi10 = Mix->fugacity_coefficient(0);
        double phi11 = Mix->fugacity_coefficient(1);

        double rr = 0;
    }
    if (0)
    {
        int N = 2;
        std::vector<long double> z(N, 1.0/N);
        double Q = 0, T = 250, p = 300000;

        std::tr1::shared_ptr<AbstractState> Mix(AbstractState::factory(std::string("HEOS"), std::string("Ethane,n-Propane")));

        Mix->set_mole_fractions(z);

        for (double T = 210; ;T += 0.1)
        {
            Mix->update(QT_INPUTS, Q, T);
            std::cout << format(" %g %g\n",Mix->p(),Mix->T());
        }
    }
    if(0)
    {
        time_t t1,t2;

        std::size_t N = 1000000;
        std::tr1::shared_ptr<AbstractState> State(AbstractState::factory(std::string("HEOS"), std::string("Water")));

        double p = State->p();
        double summer = 0;
        t1 = clock();
        for (std::size_t ii = 0; ii < N; ++ii)
        {
            //AbstractState *State = new REFPROPBackend("Methane");
            //summer += EOS->dalphar_dDelta(0.7,1.3);
            /*for (int i = 0; i < 50; i++)
            {
                summer += exp(1.3+1e-10*ii+1e-10*i);
            }
            summer += log(0.7-1e-10*ii);*/
            //summer += log(1.3);
            State->update(PT_INPUTS,101325,300);
            summer += State->p();
        }
        t2 = clock();
        double elap = ((double)(t2-t1))/CLOCKS_PER_SEC/((double)N)*1e6;
        printf("%g %g\n",elap, summer/((double)N));
        double eee = 0;
        return 0;
    }



    if (0)
    {
        std::tr1::shared_ptr<AbstractState> State(AbstractState::factory(std::string("REFPROP"), std::string("Methane|Ethane")));

        std::vector<long double> x(2,0.5);
        State->set_mole_fractions(x);
        State->update(DmassT_INPUTS,1,250);
        double hh = State->hmolar();
        double mu = State->viscosity();
        double sigma = State->surface_tension();
    }
    if (0)
    {
        time_t t1,t2;
        t1 = clock();
        long N = 100000;
        for (long ii = 0; ii < N; ii++)
        {
            std::tr1::shared_ptr<AbstractState> State(AbstractState::factory(std::string("REFPROP"), std::string("Methane")));
            //AbstractState *State = new REFPROPBackend("Methane");
        }
        t2 = clock();
        double elap = ((double)(t2-t1))/CLOCKS_PER_SEC/((double)N)*1e6;
        printf("%g\n",elap);
    }

    if(0)
    {
        std::tr1::shared_ptr<AbstractState> State(AbstractState::factory(std::string("REFPROP"), std::string("Methane")));

        State->update(DmassT_INPUTS,1,300);
        double hh = State->hmolar();
        double mu = State->viscosity();

        time_t t1,t2;
        t1 = clock();
        for (long ii = 0; ii < 1000000; ii++)
        {
            State->update(PQ_INPUTS,300000,1-ii*1e-6);
            //State->update(DmassT_INPUTS,1-ii*1e-10,180);
            //double hh1 = State->hmolar();
            //double mu2 = State->viscosity();
        }
        t2 = clock();
        double elap = ((double)(t2-t1))/CLOCKS_PER_SEC;
        printf("%g\n",elap);
    }
}
