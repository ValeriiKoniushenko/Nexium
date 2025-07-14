import { useState, useEffect } from 'react';
import type { LayoutConfig } from 'golden-layout';

export type Workspace = {
  id: string;
  name: string;
  layout: LayoutConfig;
};

const STORAGE_KEY = 'editor_workspaces';

export const defaultLayout: LayoutConfig = {
  root: {
    type: 'row',
    content: [
      {
        type: 'column',
        width: 50,
        content: [
          {
            type: 'component',
            componentType: 'Inspector',
            title: 'Inspector',
            height: 60,
          },
          {
            type: 'component',
            componentType: 'Console',
            title: 'Console',
            height: 40,
          },
        ],
      },
      {
        type: 'component',
        componentType: 'Scene',
        title: 'Scene',
        width: 50,
      },
    ],
  },
};

export const useWorkspaces = () => {
  const [workspaces, setWorkspaces] = useState<Workspace[]>([]);
  const [activeId, setActiveId] = useState<string>('');

  useEffect(() => {
    const stored = localStorage.getItem(STORAGE_KEY);
    if (stored) {
      const parsed: Workspace[] = JSON.parse(stored);
      setWorkspaces(parsed);
      if (parsed.length) setActiveId(parsed[0].id);
    } else {
      const defaultWs = {
        id: 'default',
        name: 'Default Workspace',
        layout: defaultLayout,
      };
      setWorkspaces([defaultWs]);
      setActiveId('default');
    }
  }, []);

  const activeWorkspace = workspaces.find((w) => w.id === activeId);

  const saveLayout = (layout: LayoutConfig) => {
    const updated = workspaces.map((w) => (w.id === activeId ? { ...w, layout } : w));
    setWorkspaces(updated);
    localStorage.setItem(STORAGE_KEY, JSON.stringify(updated));
  };

  const createWorkspace = () => {
    const id = Date.now().toString();
    const name = `Workspace ${workspaces.length + 1}`;
    const newWorkspace: Workspace = {
      id,
      name,
      layout: defaultLayout,
    };
    const updated = [...workspaces, newWorkspace];
    setWorkspaces(updated);
    setActiveId(id);
    localStorage.setItem(STORAGE_KEY, JSON.stringify(updated));
  };

  return {
    workspaces,
    activeId,
    activeWorkspace,
    setActiveId,
    saveLayout,
    createWorkspace,
  };
};
