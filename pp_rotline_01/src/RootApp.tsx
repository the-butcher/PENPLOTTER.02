import { useEffect, useRef, useState } from 'react';
import './App.css';
import PlotRootComponent from './components/PlotRootComponent';
import type { IPlotRootProps } from './components/IPlotRootProps';
import { ObjectUtil } from './util/ObjectUtil';
import type { ILineProps } from './components/ILineProps';
import type { ICoordPlanar } from './components/ICoordPlanar';

function RootApp() {

  useEffect(() => {
    console.log('✨ building RootApp');

    const centerX = 100;
    const centerY = 100;
    let radius = 60;

    const coords: ICoordPlanar[] = [];
    for (let deg = 0; deg <= 1440; deg += 5) {
      const rad = deg * Math.PI / 180;
      coords.push({
        x: centerX + Math.cos(rad) * radius,
        y: centerY + Math.sin(rad) * radius,
        z: 0,
        r: deg * 1.065
      });
      radius *= 0.997;
    }

    plotRootPropsRef.current = {
      ...plotRootPropsRef.current,
      lines: [
        {
          id: ObjectUtil.createId(),
          width: 6,
          coords
        }
      ]
    };
    setPlotRootProps(plotRootPropsRef.current);
  }, []);

  const plotRootPropsRef = useRef<IPlotRootProps>({
    bounds: {
      width: 297,
      height: 210
    },
    lines: [
      {
        id: ObjectUtil.createId(),
        width: 5,
        coords: [
          {
            x: 20,
            y: 20,
            z: 0,
            r: 0,
          },
          {
            x: 120,
            y: 40,
            z: 0,
            r: 90,
          },
          {
            x: 100,
            y: 80,
            z: 0,
            r: 45,
          }
        ]
      }
    ]
  });
  const [plotRootProps, setPlotRootProps] = useState<IPlotRootProps>(plotRootPropsRef.current)

  return (
    <PlotRootComponent {...plotRootProps} />
  )
}

export default RootApp
