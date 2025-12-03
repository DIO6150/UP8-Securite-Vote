import tkinter as tk
import client
from tkinter import messagebox
from PIL import Image, ImageTk
import queue

promise = queue.Queue()
info = ""
def f():
	return None
func = f

def ouvrir_fenetre_principale():
    """Crée et affiche la fenêtre principale avec les 3 boutons et leurs icônes."""

    fenetre_principale = tk.Tk()
    fenetre_principale.title("Système sécurisé de vote")
    fenetre_principale.geometry("400x300")

    def charger_icone(chemin, taille=(30, 30)):
        try:
            img = Image.open(chemin)
            img = img.resize(taille, Image.Resampling.LANCZOS)
            return ImageTk.PhotoImage(img)
        except Exception as e:
            print(f"Erreur chargement image {chemin}: {e}")
            return None

    ##client.candidats()
    candidats = ["Ariana Grande","Bob Lennon","Charlie Chaplin","David Bowie"] ##client.read()
    choix_vote = "01"
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
        text="✗ Quitter", 
        command=fenetre_principale.destroy,
        compound="left",
        padx=10,
        bg="#ffcccc"
    )
    bouton_quitter.pack(pady=20, fill="x", padx=50)

    fenetre_principale.mainloop()


def creer_fenetre_login():
    """Crée et affiche la fenêtre de login initiale."""
    
    login_fenetre = tk.Tk()
    login_fenetre.title("Connexion")
    login_fenetre.geometry("350x200")

    def update():
        global info
        info = client.read()
        if(info != "E"):
           func()
        login_fenetre.after(500, update)
    def verifier_login():
        global info
        global func
        identifiant = entry_id.get()
        mdp = entry_mdp.get()
        client.login(identifiant, mdp)
        def log():
           login = False
           code = info
           print(code)
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
              client.disconnect()
              login_fenetre.destroy()

           if login:
              print("Connexion réussie !")
              login_fenetre.destroy() 
              ouvrir_fenetre_principale()
           else:
              label_erreur.config(text="Identifiant incorrect", fg="red")
        func = log

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

    login_fenetre.after(100, update)
    login_fenetre.mainloop()


def check_connection():
    """Tente de se connecter au serveur. Si échec, affiche un message et réessaie."""
    if client.connect():
        creer_fenetre_login()
    else:
        wait_window = tk.Tk()
        wait_window.title("Connexion...")
        wait_window.geometry("400x150")
        
        lbl = tk.Label(wait_window, text="Connexion échouée...\nNouvel essai dans 3 secondes", font=("Arial", 12))
        lbl.pack(expand=True)
        
        def retry():
            wait_window.destroy()
            check_connection()
            
        wait_window.after(3000, retry)
        wait_window.mainloop()

check_connection()
