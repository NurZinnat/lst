#include "window.h"

void
Window::keyPressEvent (QKeyEvent *event)
{
  switch (event->key ())
    {
    case Qt::Key_0:
      func_id = (func_id + 1) % 7;
      f = get_func (func_id);
      f_name = get_func_name (func_id);
      poly_k = func_id + 1;
      if (poly_k < 2)
        poly_k = 2;
      if (poly_k > MAX_K)
        poly_k = MAX_K;
      if (func_id > 4)
        poly_k = 3;
      update_max_abs ();
      sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
      spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
      spl.build ();
      update ();
      break;

    case Qt::Key_1:
      display_mode = (display_mode + 1) % 4;
      update ();
      break;

    case Qt::Key_2:
      {
        double mid = (a + b) / 2;
        double half = (b - a) / 4;
        if (half / n < 1e-3)
          break;
        a = mid - half;
        b = mid + half;
        update_max_abs ();
        sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
        spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
        spl.build ();
        update ();
      }
      break;

    case Qt::Key_3:
      {
        double mid = (a + b) / 2;
        double len = b - a;
        a = mid - len;
        b = mid + len;
        update_max_abs ();
        sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
        spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
        spl.build ();
        update ();
      }
      break;

    case Qt::Key_4:
      n *= 2;
      if (n > MAX_N)
        {
          n /= 2;
          break;
        }
      if ((b - a) / n < 1e-3)
        {
          // n /= 2;
          // break;
        }
      sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
      spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
      spl.build ();
      update ();
      break;

    case Qt::Key_5:
      if (n > 2)
        n /= 2;
      sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
      spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
      spl.build ();
      update ();
      break;

    case Qt::Key_6:
      p++;
      sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
      spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
      spl.build ();
      update ();
      break;

    case Qt::Key_7:
      p--;
      sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
      spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
      spl.build ();
      update ();
      break;

    case Qt::Key_Plus:
    case Qt::Key_Equal:
      if (poly_k < MAX_K)
        {
          poly_k++;
          sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
          update ();
        }
      break;

    case Qt::Key_Minus:
      if (poly_k > 2)
        {
          poly_k--;
          sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
          update ();
        }
      break;

    case Qt::Key_Escape:
      close ();
      break;

    default:
      QWidget::keyPressEvent (event);
    }
}
