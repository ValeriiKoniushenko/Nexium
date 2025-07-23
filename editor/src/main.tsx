import { useEffect, useState } from 'react';
import ReactDOM from 'react-dom/client';
import { App } from './App';
import { ThemeProvider } from 'styled-components';
import { GlobalStyle } from './styles/GlobalStyle';
import { themes } from './styles/theme';
// import { ThemeToggler } from './components';

const THEME_KEY = 'editor_theme';

const Root = () => {
  const [mode, setMode] = useState<'dark' | 'light'>('dark');

  useEffect(() => {
    const storedTheme = localStorage.getItem(THEME_KEY);
    if (storedTheme === 'light' || storedTheme === 'dark') {
      setMode(storedTheme);
    }
  }, []);

  // const toggleTheme = () => {
  //   setMode((prev) => {
  //     const newTheme = prev === 'dark' ? 'light' : 'dark';
  //     localStorage.setItem(THEME_KEY, newTheme);
  //     return newTheme;
  //   });
  // };

  return (
    <ThemeProvider theme={themes[mode]}>
      <GlobalStyle />
      <App />
    </ThemeProvider>
  );
};

ReactDOM.createRoot(document.getElementById('root')!).render(<Root />);
