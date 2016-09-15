define values: Signal<Int> :=
  variable_values("buffer.c:write_ptr")
define new_data: Events<Int> := changeOf(values)
define data_processed: Events<Unit> :=
  function_calls("buffer.c:process")

define delayed_new_data: Events<Unit> :=
  delayEventByTime(new_data, 1000000)

define consumed_in_past: Signal<Boolean> :=
  inPast(1000000, data_processed)

define error: Events<Boolean> :=
  eventNot(sample(consumed_in_past, delayed_new_data))
