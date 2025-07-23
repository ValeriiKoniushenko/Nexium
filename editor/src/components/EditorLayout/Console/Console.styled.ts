import styled from 'styled-components';

export const ConsoleFrame = styled.div`
  /* styles for #console-main-frame */
  height: 100%;
  overflow-y: hidden;
`;

export const ConsoleTopSection = styled.section`
  padding: 8px;  
  border-bottom: 2px solid #444;
  display: flex;
  justify-content: space-between;
  align-items: center;
`;

export const ConsoleTopSection_FilterHolder = styled.div`
`;

export const LogLine = styled.div`
    border-bottom: 1px solid #44444444;
`;

export const ConsoleLogList = styled.div`
    padding: 0px 8px;
    height: 100%;
    overflow-y: auto;
`;

interface ButtonProps {
    active?: boolean;
}

export const FilterLogsButton = styled.button<ButtonProps>`
    background-color: ${({ active }) => (active ? '#555' : '#222')};
    color: ${({ active }) => (active ? '#fff' : '#aaa')};
    margin-left: 8px;
    height: 25px;
    width: 25px;
    text-align: center;
    border: 1px solid #555;

    cursor: pointer;

    &:hover {
        background-color: #444;
    }
`