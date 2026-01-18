- how does the addition of an r-value affect bluetooth transmission?
  - block_planxy_f\_\_\_t is 20 bytes at the moment (4 bytes \* 5)
  - ble max message size is 512 bytes (?)
  - 512 / 20 = 25,6 -> message count currently is 24
  - with r-value block_planxy_f\_\_\_t would be 24 bytes
  - 512 / 24 = 21.3333 -> set message count to 20
  - constants in pp_core_02 :: Define:h :: COMMAND_BUFF_VALS_SIZE, pp_client :: GeometryUtil.ts :: BT\_\_\_BUFF_BLK
  - bt block code in pp_client
  - bt block code in pp_core_02
  - bresenham code in pp_core_02
  - find out if block_planxy_i64_t's changed size will also require a change in buffer size
  - all Coords:: functions need to be revisited

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
