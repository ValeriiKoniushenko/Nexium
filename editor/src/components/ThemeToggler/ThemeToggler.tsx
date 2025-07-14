import { useEffect, useState } from 'react';
import { ToggleButton } from './ThemeToggler.styled';

type Props = {
  toggleTheme: () => void;
  currentTheme: 'light' | 'dark';
};

export const ThemeToggler = ({ toggleTheme, currentTheme }: Props) => {
  const [icon, setIcon] = useState('🌙');

  useEffect(() => {
    setIcon(currentTheme === 'light' ? '🌙' : '☀️');
  }, [currentTheme]);

  return <ToggleButton onClick={toggleTheme}>{icon}</ToggleButton>;
};
