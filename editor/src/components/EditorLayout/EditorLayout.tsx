import { useEffect, useRef } from 'react';
import { GoldenLayout, ComponentContainer, LayoutConfig } from 'golden-layout';
import 'golden-layout/dist/css/goldenlayout-base.css';
import 'golden-layout/dist/css/themes/goldenlayout-dark-theme.css';
import { Container } from './EditorLayout.styled';
import { Console } from './Console/Console'
import ReactDOM from 'react-dom/client';

type Props = {
  layoutConfig: LayoutConfig;
  onSaveLayout: (config: LayoutConfig) => void;
};

export const EditorLayout: React.FC<Props> = ({ layoutConfig }) => {
  const layoutRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!layoutRef.current) return;

    const layout = new GoldenLayout(layoutConfig, layoutRef.current);

    layout.registerComponent('Scene', (container: ComponentContainer) => {
      const el = document.createElement('div');
      el.innerText = '🎮 Scene View';
      el.style.padding = '16px';
      container.element.append(el);
    });

    layout.registerComponent('Inspector', (container: ComponentContainer) => {
      const el = document.createElement('div');
      el.innerText = '🔍 Inspector';
      el.style.padding = '16px';
      container.element.append(el);
    });

    layout.registerComponent('Console', (container: ComponentContainer) => {
      const mountPoint = document.createElement('div');
      mountPoint.style.height = "100%";
      container.element.append(mountPoint);
      ReactDOM.createRoot(mountPoint).render(<Console />);
    });

    layout.init();

    layout.loadLayout(layoutConfig);

    return () => {
      layout.destroy();
    };
  }, [layoutConfig]);

  return <Container ref={layoutRef} />;
};
