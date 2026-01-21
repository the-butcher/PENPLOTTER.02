import type { ILineProps } from "./ILineProps";
import type { IPlotBounds } from "./IPlotBounds";

export interface IPlotRootProps {
    bounds: IPlotBounds,
    lines: ILineProps[];
}