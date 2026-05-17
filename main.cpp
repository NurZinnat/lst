#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include "window.h"

int main (int argc, char *argv[])
{
  QApplication app (argc, argv);

  if (argc < 5)
    {
      fprintf (stderr, "Usage: %s a b n k\n", argv[0]);
      return -1;
    }

  double a = atof (argv[1]);
  double b = atof (argv[2]);
  int n = atoi (argv[3]);
  int func_id = atoi (argv[4]);

  if (b - a < 1e-12 || n < 2 || func_id < 0 || func_id > 6 || n > MAX_N)
    {
      fprintf (stderr, "Invalid arguments\n");
      return -2;
    }

  Window w;
  w.set_params (a, b, n, func_id);
  w.show ();

  return app.exec ();
}