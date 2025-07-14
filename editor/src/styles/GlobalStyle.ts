import { createGlobalStyle } from 'styled-components';

export const GlobalStyle = createGlobalStyle`
  *, *::before, *::after {
    box-sizing: border-box;
  }

  html, body {
    margin: 0;
    padding: 0;
    background-color: ${({ theme }) => theme.background};
    color: ${({ theme }) => theme.text};
    font-family: 'Segoe UI', sans-serif;
    transition: background-color 0.3s, color 0.3s;
  }

  a {
    color: ${({ theme }) => theme.accent};
    text-decoration: none;
  }

  button {
    font-family: inherit;
  }

  /* === Golden Layout Styles === */
  .golden-layout {
    background-color: ${({ theme }) => theme.background};
    color: ${({ theme }) => theme.text};
  }

  .lm_header {
    background-color: ${({ theme }) => theme.surface};
    border-bottom: 1px solid ${({ theme }) => theme.border};
    height: 30px;
  }

  .lm_tabs {
    background-color: ${({ theme }) => theme.surface};
  }

  .lm_tab {
    background-color: ${({ theme }) => theme.surface};
    color: ${({ theme }) => theme.text};
    padding: 4px 12px;
    transition: background-color 0.2s;
  }

  .lm_tab:hover {
    background-color: ${({ theme }) => theme.hover};
  }

  .lm_tab.lm_active {
    background-color: ${({ theme }) => theme.panel};
    font-weight: 500;
  }

  .lm_title {
    color: ${({ theme }) => theme.text};
  }

  .lm_close_tab {
    color: ${({ theme }) => theme.accent};
    opacity: 0.6;
    transition: opacity 0.2s;
  }

  .lm_close_tab:hover {
    opacity: 1;
  }

  .lm_content {
    background-color: ${({ theme }) => theme.panel};
  }

  .lm_splitter {
    background-color: ${({ theme }) => theme.border};
    transition: background-color 0.2s;
  }

  .lm_splitter:hover {
    background-color: ${({ theme }) => theme.accent};
  }

  /* Optional: scroll styling */
  .golden-layout ::-webkit-scrollbar {
    width: 8px;
  }

  .golden-layout ::-webkit-scrollbar-thumb {
    background-color: ${({ theme }) => theme.border};
    border-radius: 4px;
  }
`;
