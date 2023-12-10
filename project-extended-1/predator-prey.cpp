#include <iostream>
#include <iomanip>
#include <fstream>

#include <cmath>
#include "cpgplot.h"

using namespace std;

/************** console output **************/
#define PBWIDTH 60
#define PBSTR "============================================================"

void printProgress(double percentage) {
  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
}

/************** input values **************/
double x1, x2, y;
int n, t_end;

void get_var_inputs() {
  cout << "Type in initial population of the prey 1: ";
  cin >> x1;

  cout << "Type in initial population of the prey 2: ";
  cin >> x2;

  cout << "Type in initial population of the predator: ";
  cin >> y;

  for (;;) {
    cout << "Type in number of steps: ";
    cin >> n;

    cout << "Type in number of days: ";
    cin >> t_end;

    if ((double) t_end / (double) n < 2) break;
    else cout << "※ Timestep is too big; results won't be accurate." << endl << "※ Type in a bigger number for steps or smaller number for days." << endl;
  }
}

void default_var_inputs() {
  x1 = 500;
  x2 = 200;
  y = 20;
  n = 10000;
  t_end = 100;
}

/************** model calculation & equations **************/

double prey_1_growth = 0.5; // prey’s per capita growth rate
double prey_1_death = 0.0006; // rate of prey eaten by predator

double prey_2_growth = 0.4; // prey’s per capita growth rate
double prey_2_death = 0.0005; // rate of prey eaten by predator

double predator_growth = 0.0008; // predator's per capita growth rate (reproduction + how much prey is available to eat)
double predator_death = 0.42; // predator’s per capita death rate

double dxdt(double x, double y, double growth, double death) {
  return (growth * x) - (death * x * y);
}

double dydt(double x1, double x2, double y) {
	return (predator_growth * (x1 + x2) * y) - (predator_death * y);
}

/************** update max (for pgplot) **************/
int update_MAX(double x1, double x2, double y, int max) {
  if (x1 > y && x1 > x2 && x1 > max) return (int) x1;
  else if (x2 > y && x2 > x1 && x2 > max) return (int) x2;
  else if (y > x1 && y > x2 && y > max) return (int) y;
  return max;
}


/************** main program **************/

int main()
{
  ofstream outputfile_raw;
  ofstream outputfile_peaks;

  outputfile_raw.open("project-extended-1-raw.txt");
  outputfile_peaks.open("project-extended-1-peaks.txt");

  /* get x1, x2, y, n, t_end */
  // get_var_inputs();
  default_var_inputs();
  
  /* initialize values */
  double t = 0;
  double t_delta = (double) t_end / (double) n;
  int MAX = 0;

  /* step counter */
  int i = 0;

  /* pgplot variables */
  float tp[n+1], x1p[n+1], x2p[n+1], yp[n+1];

  outputfile_raw << "prey 1 growth rate = " << prey_1_growth << endl << "prey 1 death rate = " << prey_1_death << endl
        << "prey 2 growth rate = " << prey_2_growth << endl << "prey 2 death rate = " << prey_2_death << endl 
        << "predator growth rate = " << predator_growth << endl << "predator death rate = " << predator_death << endl
        << "x1 = " << x1 << endl << "x2 = " << x2 << endl << "y = " << y << endl << "n = " << n << endl << "t (days) = " << t_end << endl << endl
        << "| " << setw(10) << "timestep | "
        << setw(15) << "x1 | "
        << setw(15) << "x2 | "
        << setw(15) << "y | "
        << endl << "|" << setw(56) << setfill('-') << "|" << endl;

  outputfile_peaks << "prey 1 growth rate = " << prey_1_growth << endl << "prey 1 death rate = " << prey_1_death << endl
        << "prey 2 growth rate = " << prey_2_growth << endl << "prey 2 death rate = " << prey_2_death << endl 
        << "predator growth rate = " << predator_growth << endl << "predator death = " << predator_death << endl
        << "x1 = " << x1 << endl << "x2 = " << x2 << endl << "y = " << y << endl << "n = " << n << endl << "t (days) = " << t_end << endl << endl
        << "| " << setw(10) << "timestep | "
        << setw(15) << "x1 | "
        << setw(15) << "x2 | "
        << setw(15) << "y | "
        << endl << "|" << setw(56) << setfill('-') << "|" << endl;

  /* rk4 process */
  for (;;) {
    tp[i] = t;
    x1p[i] = x1;
    x2p[i] = x2;
    yp[i] = y;

    /* record to output.txt if peak of x1 */
    if (x1p[i-1] > x1p[i] && x1p[i-1] > x1p[i-2]) {
      outputfile_peaks << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << x1 << " | "
          << setw(12) << "" << " | "
          << setw(12) << "" << " | " << endl;
    }

    /* record to output.txt if peak of x2 */
    if (x2p[i-1] > x2p[i] && x2p[i-1] > x2p[i-2]) {
      outputfile_peaks << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << "" << " | "
          << setw(12) << x2 << " | "
          << setw(12) << "" << " | " << endl;
    }

    /* record to output.txt if peak of y */
    if (yp[i-1] > yp[i] && yp[i-1] > yp[i-2]) {
      outputfile_peaks << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << "" << " | "
          << setw(12) << "" << " | "
          << setw(12) << y << " | " << endl;
    }

    outputfile_raw << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << x1 << " | "
          << setw(12) << x2 << " | "
          << setw(12) << y << " | " << endl;

    printProgress(t / t_end);

    if (i >= n) break;

    t += t_delta;

    double x1_k1 = dxdt(x1, y, prey_1_growth, prey_1_death);
    double x2_k1 = dxdt(x1, y, prey_2_growth, prey_2_death);
    double y_k1 = dydt(x1, x2, y);

    double x1_k2 = dxdt(x1 + (0.5 * x1_k1 * t_delta), y + (0.5 * y_k1 * t_delta), prey_1_growth, prey_1_death);
    double x2_k2 = dxdt(x2 + (0.5 * x2_k1 * t_delta), y + (0.5 * y_k1 * t_delta), prey_2_growth, prey_2_death);
    double y_k2 = dydt(x1 + (0.5 * x1_k1 * t_delta), x2 + (0.5 * x2_k1 * t_delta), y + (0.5 * y_k1 * t_delta));

    double x1_k3 = dxdt(x1 + (0.5 * x1_k2 * t_delta), y + (0.5 * y_k2 * t_delta), prey_1_growth, prey_1_death);
    double x2_k3 = dxdt(x2 + (0.5 * x2_k2 * t_delta), y + (0.5 * y_k2 * t_delta), prey_2_growth, prey_2_death);
    double y_k3 = dydt(x1 + (0.5 * x1_k2 * t_delta), x2 + (0.5 * x2_k2 * t_delta), y + (0.5 * y_k2 * t_delta));

    double x1_k4 = dxdt(x1 + (t_delta * x1_k3), y + (t_delta * y_k3), prey_1_growth, prey_1_death);
    double x2_k4 = dxdt(x2 + (t_delta * x2_k3), y + (t_delta * y_k3), prey_2_growth, prey_2_death);
    double y_k4 = dydt(x1 + (t_delta * x1_k3), x2 + (t_delta * x2_k3), y + (t_delta * y_k3));
    
    double new_x1 = x1 + (t_delta * (x1_k1 + (2*x1_k2) + (2*x1_k3) + x1_k4) / (double) 6);
    double new_x2 = x2 + (t_delta * (x2_k1 + (2*x2_k2) + (2*x2_k3) + x2_k4) / (double) 6);
    double new_y = y + (t_delta * (y_k1 + (2*y_k2) + (2*y_k3) + y_k4) / (double) 6);

    if (new_x1 < 0) x1 = 0;
    else x1 = new_x1;

    if (new_x2 < 0) x2 = 0;
    else x2 = new_x2;

    if (new_y < 0) y = 0;
    else y = new_y;

    MAX = update_MAX(x1, x2, y, MAX);

    i++;
  }

  printProgress(1);
  cout << endl;

  outputfile_raw.close();
  outputfile_peaks.close();

  /*-----------------------------------------------------------------------
     pgplot portion
  -----------------------------------------------------------------------*/

  /* Open a plot window */
  if (!cpgopen("/XWINDOW")) return 1;

  /* Set-up plot axes */
  cpgenv(0.,t_end,0,MAX,0,1);
  /* Label axes */
  cpglab("Time (in days)", "Population", "Lotka-Volterra Predator-Prey Model");

  /* Plot prey 1 population */
  cpgsci(7); /* yellow */
  cpgline(n+1,tp,x1p);

  /* Plot prey 2 population */
  cpgsci(3); /* green */
  cpgline(n+1,tp,x2p);

  /* Plot predator population */
  cpgsci(2); /* red */
  cpgline(n+1,tp,yp);

  /* Pause and then close plot window */
  cpgclos();
}
