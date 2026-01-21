- if the r-value is not considered in length calculation, there can be cases where length is 0 and no proper duration can be calculated

- [x] create a new struct having int32 planar values (scaled) and another Device::accept method for that type
  - [x] work step by step and replace further doubles with fixed numbers
  - [x] revisit and understand the on the fly frequency calculation in Device::pulse

- for the font
  - each character could have a base shape (TODO :: check if possible)
  - depending on preceeding character (including none), there may be an "in" element
  - depending on proceeding character (including none), there may be an "out" element
  -

===========================================================================================================

Determine max planar speed with respect to XY(AB)

There needs to be a safeguard against too high rotation delta / millimeter

- in pp_client
- in pp_core_2 speeds need to be limited if the r-value / mm exceeds a certain value (there is calculation in GeometryUtil.linepathsToPlotpaths(linepaths: ILinePath[], maxMMS: number): ILine3D[])

===========================================================================================================

AB-Motors:

- https://www.omc-stepperonline.com/de/e-serie-nema-17-bipolar-0-9deg-30ncm-42-48oz-in-1-5a-42x42x38mm-4-draehte-17me15-1504s

- 0.9deg step
- x4 base microstepping
- 360 \* 4 / 0.9 => 3200 steps for a full rotation

===========================================================================================================

Z-Motor:

- https://www.omc-stepperonline.com/de/runde-nema-14-bipolar-0-9deg-8ncm-11-33oz-in-0-65a-36x20mm-4-draehte-14hr08-0654s
- 0.9deg step
- x8 microstepping (high, high, low)
- 360 \* 8 / 0.9 => 3200 steps for a full rotation
- 0.64 seconds for one rotation at 5000Hz
- lever length is 17.2mm, one full rotation at that length would be ~108mm (to be tested)
- max speed would be 108 / 0.64 => 169mm/s

===========================================================================================================

R-Motor:

- https://www.omc-stepperonline.com/de/nema-8-bipolar-1-8deg-1-4ncm-1-98oz-in-0-2a-20x20x28mm-4-draehte-8hs11-0204s
- 1.8deg step
- x8 microstepping (high, high, low)
- 360 \* 8 / 1.8 => 1600 steps for a full motor rotation
- 1600 step \* 30 / 18 => 2666.6667 steps for a full pen rotation

- 0.5333 seconds for one pen rotation at 5000Hz
- at a virtual length of 360 / (2PI) => 57,295779 one full pen rotation would be 57,295779 \* 2 \* PI => 360mm
- max virtual speed would be 360 / 0.5333 => 675mm/s or 360 / 0.5333 => 675deg/second
