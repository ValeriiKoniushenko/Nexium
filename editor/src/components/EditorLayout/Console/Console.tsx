import React, { useEffect, useState } from 'react';
import { ConsoleFrame, ConsoleTopSection, LogLine, ConsoleLogList, ConsoleTopSection_FilterHolder } from './Console.styled';
import { FilterLog } from './FilterLogs';

export const Console: React.FC = () => {
  const [logs, setLogs] = useState<string[]>([]);
  const [error, setError] = useState<string | null>(null);

  const addLog = () => {
    setLogs(prev => [...prev, 'New dummy log']);
  };
  
  const fetchLogs = async () => {
    try {
      const response = await fetch('http://127.0.0.1:61005/api/logs');
      if (!response.ok)
        throw new Error(`HTTP error ${response.status}`);
      const data = await response.text(); // or response.json() if server returns JSON
      const lines = data.split('\n').filter(line => line.trim() !== '');
      setLogs(lines);
    } catch (err) {
      setError((err as Error).message);
    }
  };

  useEffect(() => {
    const interval = setInterval(fetchLogs, 5000);
    return () => clearInterval(interval);
  }, []);



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
        {error && <div style={{ color: 'red' }}>Error: {error}</div>}
        {logs.map((log, i) => (
          <LogLine key={i}>{log}</LogLine>
        ))}
      </ConsoleLogList>
    </ConsoleFrame>
  );
};
