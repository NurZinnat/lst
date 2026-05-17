#ifndef WINDOW_H
#define WINDOW_H

#include "functions.h"
#include "sliding_polynomial.h"
#include "spline.h"
#include <QKeyEvent>
#include <QPainter>
#include <QWidget>

class Window : public QWidget
{
  Q_OBJECT

  double a{}, b{};
  int n{};
  int func_id{};
  double (*f) (double);
  const char *f_name;
  int display_mode{};
  int p{};
  double max_abs{};
  sliding_polynomial sp{};
  spline spl{};
  int poly_k{};

public:
  Window (QWidget *parent = nullptr);
  void set_params (double _a, double _b, int _n, int _func_id);

protected:
  void paintEvent (QPaintEvent *event);
  void keyPressEvent (QKeyEvent *event);

private:
  QPointF l2g (double x, double y, double min_y, double max_y);
  void find_min_max (double &min_y, double &max_y);
  void update_max_abs ();
  void draw_method1 (QPainter &painter, double min_y, double max_y);
  void draw_method2 (QPainter &painter, double min_y, double max_y);
  void draw_method1_error (QPainter &painter, double min_y, double max_y);
  void draw_method2_error (QPainter &painter, double min_y, double max_y);
  double calc_method1_error ();
  double calc_method2_error ();
};

#endif // WINDOW_H