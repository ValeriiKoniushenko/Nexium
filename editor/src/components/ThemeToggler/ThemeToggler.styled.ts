import styled from 'styled-components';

export const ToggleButton = styled.button`
  display: block;
  margin-left: auto;
  background: ${({ theme }) => theme.panel};
  color: ${({ theme }) => theme.text};
  border: 1px solid ${({ theme }) => theme.border};
  border-radius: 50%;
  width: 26px;
  height: 26px;
  cursor: pointer;
  font-size: 18px;
  display: flex;
  align-items: center;
  justify-content: center;

  &:hover {
    background: ${({ theme }) => theme.hover};
  }
`;
