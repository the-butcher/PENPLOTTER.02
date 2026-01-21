import type { ICoordPlanar } from "./ICoordPlanar";

export interface ILineProps {
    id: string;
    /**
     * pen-width in mm
     */
    width: number;
    coords: ICoordPlanar[];
}