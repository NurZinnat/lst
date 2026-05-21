#ifndef EXECUTION_STATUS_H
#define EXECUTION_STATUS_H

enum class execution_status
{
  success,
  memory,
  runtime,
  read,
  open,
  inval_arg,
  small_divider
};

#endif // EXECUTION_STATUS_H
