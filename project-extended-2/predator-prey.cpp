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
double x, y, z;
int n, t_end;

void get_var_inputs() {
  cout << "Type in initial population of the prey: ";
  cin >> x;

  cout << "Type in initial population of the predator 1: ";
  cin >> y;

  cout << "Type in initial population of the predator 2: ";
  cin >> z;

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
  y = 100;
  z = 100;
  n = 20000;
  t_end = 500;
}

/************** model calculation & equations **************/

double prey_growth = 0.5; // prey’s per capita growth rate
double prey_death = 0.002; // rate of prey eaten by predator

double predator_1_growth = 0.00025; // predator's per capita growth rate (reproduction + how much prey is available to eat)
double predator_1_death = 0.11; // predator’s per capita death rate

double predator_2_growth = 0.00023; // predator's per capita growth rate (reproduction + how much prey is available to eat)
double predator_2_death = 0.1; // predator’s per capita death rate


double dxdt(double x, double y, double z) {
  return (prey_growth * x) - (prey_death * x * (y + z)); // pred1 (y) and pred2 (z) both eat prey (x)
}

double dydt(double x, double y, double growth, double death) {
	return (growth * x * y) - (death * y);
}

/************** update max (for pgplot) **************/
int update_MAX(double x, double y, double z, int max) {
  if (x > y && x > z && x > max) return (int) x;
  else if (y > x && y > z && y > max) return (int) y;
  else if (z > x && z > y && z > max) return (int) z;
  return max;
}


/************** main program **************/

int main()
{
  ofstream outputfile_raw;
  ofstream outputfile_peaks;

  outputfile_raw.open("project-extended-2-raw.csv");
  outputfile_peaks.open("project-extended-2-peaks.csv");

  /* get x, y, z, n, t_end */
  // get_var_inputs();
  default_var_inputs();
  
  /* initialize values */
  double t = 0;
  double t_delta = (double) t_end / (double) n;
  int MAX = 0;

  /* step counter */
  int i = 0;

  /* pgplot variables */
  float tp[n+1], xp[n+1], yp[n+1], zp[n+1];

  outputfile_raw << "prey growth rate," << prey_growth << endl << "prey death rate," << prey_death << endl
        << "predator 1 growth rate," << predator_1_growth << endl << "predator 1 death rate," << predator_1_death << endl
        << "predator 2 growth rate," << predator_2_growth << endl << "predator 2 death rate," << predator_2_death << endl
        << "x," << x << endl << "y," << y << endl << "z," << z << endl << "n," << n << endl << "t (days)," << t_end << endl << endl
        << "timestep,x,y,z" << endl;

  outputfile_peaks << "prey growth rate," << prey_growth << endl << "prey death rate," << prey_death << endl
        << "predator 1 growth rate," << predator_1_growth << endl << "predator 1 death rate," << predator_1_death << endl
        << "predator 2 growth rate," << predator_2_growth << endl << "predator 2 death rate," << predator_2_death << endl
        << "x," << x << endl << "y," << y << endl << "z," << z << endl << "n," << n << endl << "t (days)," << t_end << endl << endl
        << "timestep,x,y,z" << endl;

  /* rk4 process */
  for (;;) {
    tp[i] = t;
    xp[i] = x;
    yp[i] = y;
    zp[i] = z;

    /* record to output if peak of x */
    if (xp[i-1] > xp[i] && xp[i-1] > xp[i-2]) {
      outputfile_peaks << t << "," << x << ",," << endl;
    }

    /* record to output if peak of y */
    if (yp[i-1] > yp[i] && yp[i-1] > yp[i-2]) {
      outputfile_peaks << t << ",," << y << "," << endl;
    }

    /* record to output if peak of z */
    if (zp[i-1] > zp[i] && zp[i-1] > zp[i-2]) {
      outputfile_peaks << t << ",,," << z << endl;
    }

    outputfile_raw << t << "," << x << "," << y << "," << z << endl;

    printProgress(t / t_end);

    if (i >= n) break;

    t += t_delta;

    double x_k1 = dxdt(x, y, z);
    double y_k1 = dydt(x, y, predator_1_growth, predator_1_death);
    double z_k1 = dydt(x, z, predator_2_growth, predator_2_death);

    double x_k2 = dxdt(x + (0.5 * x_k1 * t_delta), y + (0.5 * y_k1 * t_delta), z + (0.5 * z_k1 * t_delta));
    double y_k2 = dydt(x + (0.5 * x_k1 * t_delta), y + (0.5 * y_k1 * t_delta), predator_1_growth, predator_1_death);
    double z_k2 = dydt(x + (0.5 * x_k1 * t_delta), z + (0.5 * z_k1 * t_delta), predator_2_growth, predator_2_death);

    double x_k3 = dxdt(x + (0.5 * x_k2 * t_delta), y + (0.5 * y_k2 * t_delta), z + (0.5 * z_k2 * t_delta));
    double y_k3 = dydt(x + (0.5 * x_k2 * t_delta), y + (0.5 * y_k2 * t_delta), predator_1_growth, predator_1_death);
    double z_k3 = dydt(x + (0.5 * x_k2 * t_delta), z + (0.5 * z_k2 * t_delta), predator_2_growth, predator_2_death);

    double x_k4 = dxdt(x + (t_delta * x_k3), y + (t_delta * y_k3), z + (t_delta * z_k3));
    double y_k4 = dydt(x + (t_delta * x_k3), y + (t_delta * y_k3), predator_1_growth, predator_1_death);
    double z_k4 = dydt(x + (t_delta * x_k3), z + (t_delta * z_k3), predator_2_growth, predator_2_death);
    
    double new_x = x + (t_delta * (x_k1 + (2*x_k2) + (2*x_k3) + x_k4) / (double) 6);
    double new_y = y + (t_delta * (y_k1 + (2*y_k2) + (2*y_k3) + y_k4) / (double) 6);
    double new_z = z + (t_delta * (z_k1 + (2*z_k2) + (2*z_k3) + z_k4) / (double) 6);

    if (new_x < 0) x = 0;
    else x = new_x;

    if (new_y < 0) y = 0;
    else y = new_y;

    if (new_z < 0) z = 0;
    else z = new_z;

    MAX = update_MAX(x, y, z, MAX);

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

  /* Plot predator 1 population */
  cpgsci(2); /* red */
  cpgline(n+1,tp,yp);

  /* Plot predator 2 population */
  cpgsci(5); /* blue */
  cpgline(n+1,tp,zp);

  /* Pause and then close plot window */
  cpgclos();
}
