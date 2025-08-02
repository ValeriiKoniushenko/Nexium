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

// This Interface is sync with C++ struct: LogQueue::Log
interface ILog {
  author: string;
  authorPrefix: string;
  message: string;
  time: number;
  level: string;
}

export const Console: React.FC = () => {
  const [logs, setLogs] = useState<string[]>([]);

  function addRawLog(text: string) {
    setLogs((prev) => [...prev, text]);
  }

  function addLog(log: ILog) {
    let logString: string = '';

    let prettyDate = new Date(log.time).toLocaleDateString('en-GB', {
      day: '2-digit',
      month: '2-digit',
      year: '2-digit',
    });

    logString += prettyDate + ' ';
    logString += '[' + log.level + '] ';
    logString += '[' + log.author + '] ';
    if (log.authorPrefix.length != 0) {
      logString += log.authorPrefix + ' | ';
    }
    logString += log.message;

    addRawLog(logString);
  }

  async function fetchLogs() {
    try {
      const res = await axios.get('http://127.0.0.1:61005/api/logs');
      res.data.forEach((obj: ILog) => addLog(obj));
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
