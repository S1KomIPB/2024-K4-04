/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./static/**/*.{html,js}",  // Scans HTML and JS files in the static folder
    "./templates/**/*.html",    // Scans HTML files in the templates folder
    "./app/**/*.py"             // Scans Python files in the app folder if they have Tailwind classes (optional)
  ],
  theme: {
    extend: {},
  },
  plugins: [],
}
