#include "window.h"
#include <QPainter>
#include <cmath>
#include <cstdio>

void
Window::paintEvent (QPaintEvent *)
{
  QPainter painter (this);
  int W = width ();
  if (W <= 0)
    return;

  double min_y, max_y, max_abs_val = 0;
  double mid_x = (a + b) / 2;
  double err1 = 0, err2 = 0;

  if (display_mode <= 2)
    {
      find_min_max (min_y, max_y);
      max_abs_val = std::max (std::abs (max_y), std::abs (min_y));

      double range = max_y - min_y;
      double delta = (range < 1e-12) ? 0.1 : 0.01 * range;
      min_y -= delta;
      max_y += delta;

      if (min_y > 0)
        min_y = -delta;
      if (max_y < 0)
        max_y = delta;

      double hx = (b - a) / W;

      painter.setPen (QPen (Qt::black, 2));
      double x1 = a, y1 = f (a);
      if (std::abs (x1 - mid_x) < 1e-12)
        y1 += p * 0.1 * max_abs;
      for (int i = 1; i <= W; i++)
        {
          double x2 = a + i * hx, y2 = f (x2);
          if (std::abs (x2 - mid_x) < 1e-12)
            y2 += p * 0.1 * max_abs;
          painter.drawLine (l2g (x1, y1, min_y, max_y),
                            l2g (x2, y2, min_y, max_y));
          x1 = x2;
          y1 = y2;
        }

      if (display_mode == 0 || display_mode == 2)
        {
          painter.setPen (QPen (Qt::green, 2));
          draw_method1 (painter, min_y, max_y);
        }
      if (display_mode == 1 || display_mode == 2)
        {
          painter.setPen (QPen (Qt::magenta, 2));
          draw_method2 (painter, min_y, max_y);
        }

      painter.setPen (QPen (Qt::red, 1));
      painter.drawLine (l2g (a, 0, min_y, max_y), l2g (b, 0, min_y, max_y));
      painter.drawLine (l2g (0, min_y, min_y, max_y),
                        l2g (0, max_y, min_y, max_y));
    }
  else
    {
      err1 = calc_method1_error ();
      err2 = calc_method2_error ();

      double common_max = std::max (err1, err2);
      double delta = 0.01 * common_max;
      if (delta < 0.01)
        delta = 0.1;
      double common_min = 0;
      common_max += delta;

      draw_method1_error (painter, common_min, common_max);
      draw_method2_error (painter, common_min, common_max);

      painter.setPen (QPen (Qt::red, 1));
      painter.drawLine (l2g (a, 0, common_min, common_max),
                        l2g (b, 0, common_min, common_max));
      painter.drawLine (l2g (0, common_min, common_min, common_max),
                        l2g (0, common_max, common_min, common_max));

      max_abs_val = common_max;
    }

  painter.setPen (QPen (Qt::blue));
  painter.drawText (20, 20, QString ("mode=%1").arg (display_mode));
  painter.drawText (
      20, 40, QString ("[%1, %2]").arg (a, 0, 'f', 4).arg (b, 0, 'f', 4));
  painter.drawText (20, 60, QString ("k=%1 %2").arg (func_id).arg (f_name));

  if (display_mode == 3)
    {
      painter.setPen (QPen (Qt::blue));
      painter.drawText (20, 80, QString ("max err1=%1").arg (err1, 0, 'e', 3));
      painter.setPen (QPen (QColor (255, 105, 180)));
      painter.drawText (20, 100,
                        QString ("max err2=%1").arg (err2, 0, 'e', 3));
    }
  else
    {
      painter.drawText (20, 80,
                        QString ("max|F|=%1").arg (max_abs_val, 0, 'f', 6));
    }

  painter.drawText (20, 120,
                    QString ("n=%1  k=%2  p=%3").arg (n).arg (poly_k).arg (p));

  int ly = 150;
  if (display_mode <= 2)
    {
      painter.setPen (QPen (Qt::black, 1));
      painter.drawText (20, ly, "Black - f(x)");
      ly += 20;
    }
  if (display_mode == 0 || display_mode == 2)
    {
      painter.setPen (QPen (Qt::green, 1));
      painter.drawText (20, ly, "Green - Method 1");
      ly += 20;
    }
  if (display_mode == 1 || display_mode == 2)
    {
      painter.setPen (QPen (Qt::magenta, 1));
      painter.drawText (20, ly, "Magenta - Method 2");
      ly += 20;
    }
  if (display_mode == 3)
    {
      painter.setPen (QPen (Qt::blue, 1));
      painter.drawText (20, ly, "Blue - Error Method 1");
      ly += 20;
      painter.setPen (QPen (QColor (255, 105, 180), 1));
      painter.drawText (20, ly, "Pink - Error Method 2");
    }

  // printf ("max|F|=%e\n", max_abs_val);
  fflush (stdout);
}