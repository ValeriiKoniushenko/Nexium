import styled from 'styled-components';

export const Wrapper = styled.div`
  display: flex;
  flex-direction: column;
  height: 100vh;
  overflow: hidden;
`;

export const TopBar = styled.div`
  display: flex;
  justify-content: space-between;
  padding: 4px 12px;
  background: ${({ theme }) => theme.surface};
  border-bottom: 1px solid ${({ theme }) => theme.border};
  flex-shrink: 0;
`;

export const MiddleArea = styled.div`
  flex: 1;
  height: 100%;
  overflow: hidden;
`;

export const BottomBar = styled.div`
  height: 32px;
  background: ${({ theme }) => theme.surface};
  border-top: 1px solid ${({ theme }) => theme.border};
  flex-shrink: 0;
`;
