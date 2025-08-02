import React, { useState } from 'react';
import { FilterLogsButton } from './Console.styled';

export const enum LogLevel {
  trace = 'trace',
  debug = 'debug',
  info = 'info',
  warn = 'warn',
  err = 'err',
  critical = 'critical',
}

type Props = {
  filter: LogLevel;
};

export const FilterLog: React.FC<Props> = ({ filter }) => {
  const [isActive, setIsActive] = useState(true);

  const toggleState = () => {
    setIsActive((prev) => !prev);
  };

  return (
    <FilterLogsButton active={isActive} onClick={toggleState}>
      {filter.toString()[0].toUpperCase()}
    </FilterLogsButton>
  );
};
