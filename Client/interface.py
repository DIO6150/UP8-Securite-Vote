import tkinter as tk
##import client
from tkinter import messagebox
from PIL import Image, ImageTk

def ouvrir_fenetre_principale():
    """Crée et affiche la fenêtre principale avec les 3 boutons et leurs icônes."""

    def action_bouton1():
        print("Le Bouton 1 a été cliqué !")

    def action_bouton2():
        print("Action spéciale du Bouton 2 !")

    fenetre_principale = tk.Tk()
    fenetre_principale.title("Mon interface à 3 boutons")
    fenetre_principale.geometry("400x300")

    def charger_icone(chemin, taille=(30, 30)):
        try:
            img = Image.open(chemin)
            img = img.resize(taille, Image.Resampling.LANCZOS)
            return ImageTk.PhotoImage(img)
        except Exception as e:
            print(f"Erreur chargement image {chemin}: {e}")
            return None

    icone_quit = charger_icone("quit.png")
    ##client.candidats()
    candidats = ["Ariana Grande","Bob Lennon","Charlie Chaplin","David Bowie"] ##client.read()
    choix_vote = client.read()
    if choix_vote == "O1":
        messagebox.showinfo("Vote", "Votre vote a été pris en compte")
    else:
        messagebox.showinfo("Erreur inconnue")


    def creer_action_vote(candidat):
        ##client.vote(candidat)
        return lambda: print(f"A voté pour {candidat}")

    for candidat in candidats:
        btn = tk.Button(
            fenetre_principale,
            text=f"Voter pour {candidat}",
            command=creer_action_vote(candidat),
            padx=10,
            pady=5
        )
        btn.pack(pady=5, fill="x", padx=50)

    bouton_quitter = tk.Button(
        fenetre_principale, 
        text="Quitter", 
        command=fenetre_principale.destroy,
        image=icone_quit,
        compound="left",
        padx=10,
        bg="#ffcccc"
    )
    bouton_quitter.image = icone_quit
    bouton_quitter.pack(pady=20, fill="x", padx=50)

    fenetre_principale.mainloop()


def creer_fenetre_login():
    """Crée et affiche la fenêtre de login initiale."""
    
    login_fenetre = tk.Tk()
    login_fenetre.title("Connexion")
    login_fenetre.geometry("350x200")

    def verifier_login():
        identifiant = entry_id.get()
        mdp = entry_mdp.get()
        ##client.login(identifiant, mdp)
        login = False
        code = "O0" ##client.read()
        if code == "E":
            label_erreur.config(text="Le serveur ne réponds pas !", fg="red")
        elif code == "E2":
            label_erreur.config(text="Identifiant invalide", fg="red")
        elif code == "E3":
            label_erreur.config(text="Mot de passe invalide", fg="red")
        elif code == "E10":
            label_erreur.config(text="Vote non ouvert", fg="red")
        elif code == "O0":
            label_erreur.config(text="Connexion réussie !", fg="green")
            login = True
        else:
            label_erreur.config(text="Erreur inconnue, connexion fermée", fg="red")
            ##client.disconnect()
            login_fenetre.destroy()

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


creer_fenetre_login()
