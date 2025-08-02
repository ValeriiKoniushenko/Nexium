import React, { useEffect, useState } from 'react';
import {
  ConsoleFrame,
  ConsoleTopSection,
  LogLine,
  ConsoleLogList,
  ConsoleTopSection_FilterHolder,
} from './Console.styled';
import { FilterLog } from './FilterLogs';
import axios from 'axios';

export const Console: React.FC = () => {
  const [logs, setLogs] = useState<string[]>([]);

  function addLog(text: string) {
    setLogs((prev) => [...prev, text]);
  }

  async function fetchLogs() {
    try {
      const res = await axios.get<string>('http://127.0.0.1:61005/api/logs');
      addLog(res.data);
    } catch (err) {
      console.error('Console.fetchLogs error');
    }
  }

  useEffect(() => {
    const interval = setInterval(fetchLogs, 5000);
    return () => clearInterval(interval);
  }, []);

  return (
    <ConsoleFrame>
      <ConsoleTopSection>
        <ConsoleTopSection_FilterHolder>
          <FilterLog thisFilter="C" />
          <FilterLog thisFilter="E" />
          <FilterLog thisFilter="W" />
          <FilterLog thisFilter="I" />
          <FilterLog thisFilter="D" />
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
