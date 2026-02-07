@echo off
git config core.pager ""
git checkout gestion-de-la-production
git add .
git commit -m "Ajout du travail de production"
git push origin gestion-de-la-production
