- how would the r-value affect frequency and microstepping calculation?

  - being an angular measurement, would it be a 4-th dimension or would it have to be calculated separately?
  - create a poc in javascript
  - treat r-value like a linear measurement, define a virtual radius for pen-rotation
  - the motor could i.e. have a ONE_ROTATION_M_um value of 36000, 36mm, 1mm/10deg, 180µm/step
  - a decision on microstepping needs to be made
  - gear ratio needs to be considered (18/30)

- [x] create a new struct having int32 planar values (scaled) and another Device::accept method for that type

  - [x] work step by step and replace further doubles with fixed numbers
  - [x] revisit and understand the on the fly frequency calculation in Device::pulse

- for the font
  - each character could have a base shape (TODO :: check if possible)
  - depending on preceeding character (including none), there may be an "in" element
  - depending on proceeding character (including none), there may be an "out" element
  -
