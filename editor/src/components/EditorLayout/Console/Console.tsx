import React, { useState } from 'react';
import { ConsoleFrame, ConsoleTopSection, LogLine, ConsoleLogList, ConsoleTopSection_FilterHolder } from './Console.styled';
import { FilterLog } from './FilterLogs';

export const Console: React.FC = () => {
  const [logs, setLogs] = useState<string[]>([]);

  const addLog = () => {
    setLogs(prev => [...prev, 'New dummy log']);
  };

  return (
    <ConsoleFrame>
      <ConsoleTopSection>
        <button onClick={addLog}>Add</button>
        <ConsoleTopSection_FilterHolder>
          <FilterLog thisFilter='C' />
          <FilterLog thisFilter='E' />
          <FilterLog thisFilter='W' />
          <FilterLog thisFilter='I' />
          <FilterLog thisFilter='D' />
        </ConsoleTopSection_FilterHolder>
      </ConsoleTopSection> 
      <ConsoleLogList>
        {logs.map((log, i) => (
          <LogLine key={i}>{log}</LogLine>
        ))}
      </ConsoleLogList>
    </ConsoleFrame>
  );
};
