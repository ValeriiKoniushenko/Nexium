import { darkTheme } from './themes/dark';
import { lightTheme } from './themes/light';

export type ThemeType = typeof darkTheme;

export const themes = {
  dark: darkTheme,
  light: lightTheme,
};
