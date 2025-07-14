import { useWorkspaces } from '../../hooks';

export const WorkspaceControls: React.FC = () => {
  const { workspaces, activeId, setActiveId, createWorkspace } = useWorkspaces();

  return (
    <div style={{ display: 'flex', gap: '8px', alignItems: 'center' }}>
      <select
        value={activeId}
        onChange={(e) => setActiveId(e.target.value)}
        style={{ padding: '4px 8px' }}
      >
        {workspaces.map((ws) => (
          <option key={ws.id} value={ws.id}>
            {ws.name}
          </option>
        ))}
      </select>
      <button onClick={createWorkspace}>＋</button>
    </div>
  );
};
