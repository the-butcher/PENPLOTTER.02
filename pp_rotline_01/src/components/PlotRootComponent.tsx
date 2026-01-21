import { useEffect, useState } from "react";
import type { IPlotRootProps } from "./IPlotRootProps";
import LineComponent from "./LineComponent";

const IMAGE___SCALE = 3;

function PlotRootComponent(props: IPlotRootProps) {

    const { bounds, lines } = { ...props };

    const [viewbox, setViewbox] = useState<string>('0, 0, 1, 1');
    const [width, setWidth] = useState<number>(1);
    const [height, setHeight] = useState<number>(1);


    useEffect(() => {

        console.debug('⚙ updating plot root component (bounds)', bounds);

        // const _width = 40 / GeometryUtil.IMAGE___SCALE;
        // const _pathsHeight = _extent.yMax - _extent.yMin;
        // const _imageHeight = _extent.yMax - _extent.yMin + GeometryUtil.IMAGE_PADDING * 2;
        const _viewbox = `${0}, ${0}, ${bounds.width},${bounds.height}`;
        setViewbox(_viewbox);
        setWidth(bounds.width * IMAGE___SCALE);
        setHeight(bounds.height * IMAGE___SCALE);

        // const yMin = _extent.yMin + GeometryUtil.IMAGE_PADDING;
        // const yMax = _extent.yMin + _imageHeight - GeometryUtil.IMAGE_PADDING;
        // const _d = `M${_width - 5} ${yMin}L${_width - 2} ${yMin}L${_width - 2} ${yMax}L${_width - 5} ${yMax}`;
        // setD(_d);
        // setTextPos({
        //     x: _width - 4,
        //     y: _extent.yMin + _imageHeight / 2,
        // });
        // setText(`${_pathsHeight.toLocaleString(undefined, ObjectUtil.NUMBER_OPTIONS_DEFAULT)}mm`)

    }, [bounds]);

    return (
        <svg
            style={{
                height,
                width,
                backgroundColor: 'white'
            }}
            viewBox={viewbox}
        >
            {
                lines.map(l => <LineComponent key={l.id} {...l} />)
            }
        </svg >
    )
}

export default PlotRootComponent
