# Ashen Deck — compilation GitHub Actions

1. Crée un dépôt GitHub.
2. Envoie tout le contenu de ce dossier dans le dépôt.
3. Ouvre l'onglet **Actions**.
4. Sélectionne **Build Nintendo 3DS**.
5. Clique sur **Run workflow**.
6. Une fois terminé, ouvre le workflow réussi.
7. Dans **Artifacts**, télécharge `ashen-deck-3dsx`.
8. Décompresse l'archive téléchargée : elle contient `3ds_card_roguelike.3dsx`.
9. Copie le `.3dsx` dans `/3ds/` sur la carte SD de ta 3DS.

Le workflow utilise l'image officielle devkitPro/devkitARM pour compiler le projet dans le cloud.
