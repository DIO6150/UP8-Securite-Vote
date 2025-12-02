import tkinter as tk
from tkinter import messagebox
from PIL import Image, ImageTk  # Importation nécessaire pour gérer les images

def ouvrir_fenetre_principale():
    """Crée et affiche la fenêtre principale avec les 3 boutons et leurs icônes."""

    def action_bouton1():
        print("Le Bouton 1 a été cliqué !")

    def action_bouton2():
        print("Action spéciale du Bouton 2 !")

    fenetre_principale = tk.Tk()
    fenetre_principale.title("Mon interface à 3 boutons")
    fenetre_principale.geometry("400x300") # J'ai un peu agrandi pour les images

<<<<<<< HEAD
    # --- CHARGEMENT DES IMAGES ---
    # Fonction utilitaire pour charger et redimensionner une image proprement
    def charger_icone(chemin, taille=(30, 30)):
        try:
            img = Image.open(chemin)
            img = img.resize(taille, Image.Resampling.LANCZOS) # Redimensionner
            return ImageTk.PhotoImage(img)
        except Exception as e:
            print(f"Erreur chargement image {chemin}: {e}")
            return None
=======
    bouton1 = tk.Button(fenetre_principale, text="Cliquez-moi (Bouton 1)", command=action_bouton1)
    bouton2 = tk.Button(fenetre_principale, text="Action Spéciale (Bouton 2)", command=action_bouton2)
    # 💡 CHANGEMENT ICI : Utilisez .destroy au lieu de .quit
    bouton3 = tk.Button(fenetre_principale, text="Quitter", command=fenetre_principale.destroy)
>>>>>>> d8c641b4ce835e7384827c057460945679194722

    # Remplacez ces noms par vos propres fichiers images
    icone_btn1 = charger_icone("unknown1.jpg") 
    icone_btn2 = charger_icone("unknown2.jpg")
    icone_quit = charger_icone("quit.png")

    # --- CRÉATION DES BOUTONS ---
    
    # Bouton 1
    bouton1 = tk.Button(
        fenetre_principale, 
        text=" Cliquez-moi (Bouton 1)", 
        command=action_bouton1,
        image=icone_btn1,      # Ajout de l'image
        compound="left",       # L'image se place à GAUCHE du texte
        padx=10                # Un peu d'espace interne
    )
    # IMPORTANT : Garder une référence pour éviter que l'image ne disparaisse
    bouton1.image = icone_btn1 

    # Bouton 2
    bouton2 = tk.Button(
        fenetre_principale, 
        text=" Action Spéciale (Bouton 2)", 
        command=action_bouton2,
        image=icone_btn2,
        compound="left",
        padx=10
    )
    bouton2.image = icone_btn2

    # Bouton 3 (Quitter)
    bouton3 = tk.Button(
        fenetre_principale, 
        text=" Quitter", 
        command=fenetre_principale.destroy,  # <--- Utiliser destroy ici
        image=icone_quit,
        compound="left",
        padx=10,
        bg="#ffcccc"
    )
    bouton3.image = icone_quit

    # --- AFFICHAGE ---
    bouton1.pack(pady=15, fill="x", padx=50) # fill="x" pour uniformiser la largeur
    bouton2.pack(pady=15, fill="x", padx=50)
    bouton3.pack(pady=15, fill="x", padx=50)

    fenetre_principale.mainloop()


def creer_fenetre_login():
    """Crée et affiche la fenêtre de login initiale."""
    
    login_fenetre = tk.Tk()
    login_fenetre.title("Connexion")
    login_fenetre.geometry("350x200")

    def verifier_login():
<<<<<<< HEAD
        # Pour l'exemple, on accepte tout
=======
        identifiant = entry_id.get()
        mdp = entry_mdp.get()
>>>>>>> d8c641b4ce835e7384827c057460945679194722
        login = 1 

        if login:
            print("Connexion réussie !")
            login_fenetre.destroy() 
            ouvrir_fenetre_principale()
        else:
            label_erreur.config(text="Identifiant incorrect", fg="red")

    tk.Label(login_fenetre, text="Identifiant:").grid(row=0, column=0, padx=10, pady=10)
    entry_id = tk.Entry(login_fenetre)
    entry_id.grid(row=0, column=1, padx=10)

    tk.Label(login_fenetre, text="Mot de passe:").grid(row=1, column=0, padx=10, pady=10)
    entry_mdp = tk.Entry(login_fenetre, show="*")
    entry_mdp.grid(row=1, column=1, padx=10)

    label_erreur = tk.Label(login_fenetre, text="")
    label_erreur.grid(row=2, column=0, columnspan=2, pady=5)

    btn_connecter = tk.Button(login_fenetre, text="Se connecter", command=verifier_login)
    btn_connecter.grid(row=3, column=0, columnspan=2, pady=10)

    login_fenetre.mainloop()

<<<<<<< HEAD
if __name__ == "__main__":
    creer_fenetre_login()
=======

creer_fenetre_login()
>>>>>>> d8c641b4ce835e7384827c057460945679194722
