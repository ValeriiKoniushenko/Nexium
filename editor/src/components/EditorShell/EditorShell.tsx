import type React from 'react';
import { useWorkspaces } from '../../hooks';
import { EditorLayout } from '../EditorLayout/EditorLayout';
import { FileMenu } from '../FileMenu/FileMenu';
import { Toolbar } from '../Toolbar/Toolbar';
import { WorkspaceControls } from '../WorkspaceControls/WorkspaceControls';
import { BottomBar, MiddleArea, TopBar, Wrapper } from './EditorShell.styled';

export const EditorShell: React.FC = () => {
  const { activeWorkspace, saveLayout } = useWorkspaces();

  if (!activeWorkspace) return null;

  return (
    <Wrapper>
      <TopBar>
        <FileMenu />
        <WorkspaceControls />
      </TopBar>
      <MiddleArea>
        <EditorLayout layoutConfig={activeWorkspace.layout} onSaveLayout={saveLayout} />
      </MiddleArea>
      <BottomBar>
        <Toolbar />
      </BottomBar>
    </Wrapper>
  );
};
