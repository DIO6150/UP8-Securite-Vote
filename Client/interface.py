import tkinter as tk

def ouvrir_fenetre_principale():
    """Crée et affiche la fenêtre principale avec les 3 boutons."""

    def action_bouton1():
        print("Le Bouton 1 a été cliqué !")

    def action_bouton2():
        print("Action spéciale du Bouton 2 !")

    fenetre_principale = tk.Tk()
    fenetre_principale.title("Mon interface à 3 boutons")
    fenetre_principale.geometry("300x200")

    bouton1 = tk.Button(fenetre_principale, text="Cliquez-moi (Bouton 1)", command=action_bouton1)
    bouton2 = tk.Button(fenetre_principale, text="Action Spéciale (Bouton 2)", command=action_bouton2)
    # 💡 CHANGEMENT ICI : Utilisez .destroy au lieu de .quit
    bouton3 = tk.Button(fenetre_principale, text="Quitter", command=fenetre_principale.destroy)

    bouton1.pack(pady=10)
    bouton2.pack(pady=10)
    bouton3.pack(pady=10)

    fenetre_principale.mainloop()


def creer_fenetre_login():
    """Crée et affiche la fenêtre de login initiale."""
    
    login_fenetre = tk.Tk()
    login_fenetre.title("Connexion")
    login_fenetre.geometry("350x200")

    def verifier_login():
        identifiant = entry_id.get()
        mdp = entry_mdp.get()
        login = 1 

        if login:
            print("Connexion réussie !")
            login_fenetre.destroy() 
            ouvrir_fenetre_principale()
        else:
            label_erreur.config(text="Identifiant ou Mot de passe incorrect", fg="red")

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
