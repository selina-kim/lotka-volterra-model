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
double x, y;
int n, t_end;

void get_var_inputs() {
  cout << "Type in initial population of the prey: ";
  cin >> x;

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
  x = 500;
  y = 20;
  t_end = 180;
  n = t_end * 500;
}

/************** model calculation & equations **************/

double prey_growth = 0.8; // prey’s per capita growth rate
double prey_death = 0.002; // rate of prey eaten by predator
double predator_growth = 0.0004; // predator's per capita growth rate (reproduction + how much prey is available to eat)
double predator_death = 0.2; // predator’s per capita death rate

double dxdt(double x, double y) {
  return (prey_growth * x) - (prey_death * x * y);
}

double dydt(double x, double y) {
	return (predator_growth * x * y) - (predator_death * y);
}

/************** update max (for pgplot) **************/
int update_MAX(double x, double y, int max) {
  if (x > y && x > max) return (int) x;
  else if (y > x && y > max) return (int) y;
  return max;
}


/************** main program **************/

int main()
{
  ofstream outputfile_raw;
  ofstream outputfile_peaks;

  outputfile_raw.open("project-demo-raw.txt");
  outputfile_peaks.open("project-demo-peaks.txt");

  /* get x, y, n, t_end */
  // get_var_inputs();
  default_var_inputs();
  
  /* initialize values */
  double t = 0;
  double t_delta = (double) t_end / (double) n;
  int MAX = 0;

  /* step counter */
  int i = 0;

  /* pgplot variables */
  float tp[n+1], xp[n+1], yp[n+1];

  outputfile_raw << "prey growth rate = " << prey_growth << endl << "prey death rate = " << predator_death << endl << "predator growth rate = " << predator_growth << endl << "predator death = " << predator_death << endl
        << "x = " << x << endl << "y = " << y << endl << "n = " << n << endl << "t (days) = " << t_end << endl << endl
        << "| " << setw(10) << "timestep | "
        << setw(15) << "x | "
        << setw(15) << "y | "
        << endl << "|" << setw(41) << setfill('-') << "|" << endl;

  outputfile_peaks << "prey growth rate = " << prey_growth << endl << "prey death rate = " << predator_death << endl << "predator growth rate = " << predator_growth << endl << "predator death = " << predator_death << endl
        << "x = " << x << endl << "y = " << y << endl << "n = " << n << endl << "t (days) = " << t_end << endl << endl
        << "| " << setw(10) << "timestep | "
        << setw(15) << "x | "
        << setw(15) << "y | "
        << endl << "|" << setw(41) << setfill('-') << "|" << endl;

  /* rk4 process */
  for (;;) {
    tp[i] = t;
    xp[i] = x;
    yp[i] = y;

    /* record to output.txt if peak of x */
    if (xp[i-1] > xp[i] && xp[i-1] > xp[i-2]) {
      outputfile_peaks << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << x << " | "
          << setw(12) << "" << " | " << endl;
    }

    /* record to output.txt if peak of y */
    if (yp[i-1] > yp[i] && yp[i-1] > yp[i-2]) {
      outputfile_peaks << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << "" << " | "
          << setw(12) << y << " | " << endl;
    }

    outputfile_raw << "| " << setfill(' ') << setw(8) << t << " | "
          << setw(12) << x << " | "
          << setw(12) << y << " | " << endl;

    printProgress(t / t_end);

    if (i >= n) break;

    t += t_delta;

    double x_k1 = dxdt(x, y);
    double y_k1 = dydt(x, y);

    double x_k2 = dxdt(x + (0.5 * x_k1 * t_delta), y + (0.5 * y_k1 * t_delta));
    double y_k2 = dydt(x + (0.5 * x_k1 * t_delta), y + (0.5 * y_k1 * t_delta));

    double x_k3 = dxdt(x + (0.5 * x_k2 * t_delta), y + (0.5 * y_k2 * t_delta));
    double y_k3 = dydt(x + (0.5 * x_k2 * t_delta), y + (0.5 * y_k2 * t_delta));

    double x_k4 = dxdt(x + (t_delta * x_k3), y + (t_delta * y_k3));
    double y_k4 = dydt(x + (t_delta * x_k3), y + (t_delta * y_k3));
    
    double new_x = x + (t_delta * (x_k1 + (2*x_k2) + (2*x_k3) + x_k4) / (double) 6);
    double new_y = y + (t_delta * (y_k1 + (2*y_k2) + (2*y_k3) + y_k4) / (double) 6);

    if (new_x < 0) x = 0;
    else x = new_x;

    if (new_y < 0) y = 0;
    else y = new_y;

    MAX = update_MAX(x, y, MAX);

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

  /* Plot prey population */
  cpgsci(7); /* yellow */
  cpgline(n+1,tp,xp);

  /* Plot predator population */
  cpgsci(2); /* red */
  cpgline(n+1,tp,yp);

  /* Pause and then close plot window */
  cpgclos();
}
