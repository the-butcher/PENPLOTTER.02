import { useEffect, useState, type JSX } from "react";
import type { ILineProps } from "./ILineProps";

function LineComponent(props: ILineProps) {

    const { id, width, coords } = { ...props };

    const [fills, setFills] = useState<JSX.Element[]>([]);
    const [lineD, setLineD] = useState<string>('');

    useEffect(() => {

        console.debug('⚙ updating line component (coords)', coords);

        let command = 'M';
        let _lineD = '';
        let _fills: JSX.Element[] = [];
        for (let coordIndex = 0; coordIndex < coords.length; coordIndex++) {

            _lineD += `${command}${coords[coordIndex].x.toFixed(3)} ${coords[coordIndex].y.toFixed(3)}`;
            command = 'L';

            if (coordIndex > 0) {
                const angleA = coords[coordIndex - 1].r * Math.PI / 180;
                const angleB = coords[coordIndex].r * Math.PI / 180;
                let fillD = `M${coords[coordIndex - 1].x - Math.cos(angleA) * width / 2} ${coords[coordIndex - 1].y - Math.sin(angleA) * width / 2}`;
                fillD += `L${coords[coordIndex - 1].x + Math.cos(angleA) * width / 2} ${coords[coordIndex - 1].y + Math.sin(angleA) * width / 2}`;
                fillD += `L${coords[coordIndex].x + Math.cos(angleB) * width / 2} ${coords[coordIndex].y + Math.sin(angleB) * width / 2}`;
                fillD += `L${coords[coordIndex].x - Math.cos(angleB) * width / 2} ${coords[coordIndex].y - Math.sin(angleB) * width / 2}`;
                fillD += 'Z';
                _fills.push(<path key={`${id}_${coordIndex}`} d={fillD} fill={'#666666'} stroke={'#666666'} strokeWidth={0.75} strokeLinejoin={'round'} strokeLinecap={'round'} fillRule={'nonzero'} />);
            }

        }
        setFills(_fills);
        setLineD(_lineD);

    }, [coords]);

    return (
        <g>
            {
                fills
            }
            <path d={lineD} fill={'none'} stroke={'#000000'} strokeWidth={0.25} strokeLinejoin={'round'} strokeLinecap={'round'} />
        </g>
    )
}

export default LineComponent
