import tkinter as tk
from tkinter import font as tkfont
import client
from tkinter import messagebox
from PIL import Image, ImageTk
import queue

promise = queue.Queue()
info = ""
def f():
	return None
func = f

# Modern Color Palette
COLORS = {
    'bg_dark': '#1a1a2e',
    'bg_medium': '#16213e',
    'bg_light': '#0f3460',
    'primary': '#3282b8',
    'primary_hover': '#4a9fd8',
    'secondary': '#bbe1fa',
    'text_light': '#ffffff',
    'text_gray': '#cbd5e0',
    'success': '#51cf66',
    'error': '#ff6b6b',
    'warning': '#ffa94d',
    'card_bg': '#253248',
    'card_hover': '#2d3f5f'
}

def create_rounded_button(parent, text, command, bg_color, hover_color, fg_color='white', width=20):
    """Creates a modern button with hover effect"""
    btn = tk.Button(
        parent,
        text=text,
        command=command,
        bg=bg_color,
        fg=fg_color,
        font=('Helvetica', 11, 'bold'),
        relief=tk.FLAT,
        cursor='hand2',
        padx=20,
        pady=12,
        width=width,
        borderwidth=0
    )
    
    def on_enter(e):
        btn['bg'] = hover_color
    
    def on_leave(e):
        btn['bg'] = bg_color
    
    btn.bind("<Enter>", on_enter)
    btn.bind("<Leave>", on_leave)
    
    return btn

def create_styled_entry(parent, show=None):
    """Creates a modern styled entry field"""
    entry = tk.Entry(
        parent,
        bg=COLORS['card_bg'],
        fg=COLORS['text_light'],
        font=('Helvetica', 11),
        relief=tk.FLAT,
        insertbackground=COLORS['secondary'],
        borderwidth=2,
        highlightthickness=2,
        highlightbackground=COLORS['bg_light'],
        highlightcolor=COLORS['primary'],
        show=show if show else ''
    )
    return entry

def ouvrir_fenetre_principale():
    """Crée et affiche la fenêtre principale avec les candidats stylisés."""

    fenetre_principale = tk.Tk()
    fenetre_principale.title("🗳️ Vote Électronique")
    fenetre_principale.geometry("600x700")
    fenetre_principale.configure(bg=COLORS['bg_dark'])
    
    # Prevent resize for consistent look
    fenetre_principale.resizable(False, False)

    # Header Section
    header_frame = tk.Frame(fenetre_principale, bg=COLORS['bg_medium'], height=100)
    header_frame.pack(fill=tk.X, pady=(0, 20))
    header_frame.pack_propagate(False)
    
    title_label = tk.Label(
        header_frame,
        text="🗳️ VOTE ÉLECTRONIQUE",
        font=('Helvetica', 24, 'bold'),
        bg=COLORS['bg_medium'],
        fg=COLORS['text_light']
    )
    title_label.pack(pady=20)
    
    subtitle_label = tk.Label(
        header_frame,
        text="Sélectionnez votre candidat préféré",
        font=('Helvetica', 12),
        bg=COLORS['bg_medium'],
        fg=COLORS['text_gray']
    )
    subtitle_label.pack()

    # Candidats Section
    candidats = ["Ariana Grande", "Bob Lennon", "Charlie Chaplin", "David Bowie"]
    
    # Create scrollable frame for candidates
    canvas = tk.Canvas(fenetre_principale, bg=COLORS['bg_dark'], highlightthickness=0)
    scrollbar = tk.Scrollbar(fenetre_principale, orient="vertical", command=canvas.yview)
    scrollable_frame = tk.Frame(canvas, bg=COLORS['bg_dark'])
    
    scrollable_frame.bind(
        "<Configure>",
        lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
    )
    
    canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
    canvas.configure(yscrollcommand=scrollbar.set)
    
    def creer_action_vote(candidat):
        return lambda: [
            print(f"✓ A voté pour {candidat}"),
            messagebox.showinfo("Vote enregistré", f"Votre vote pour {candidat} a été enregistré avec succès!"),
            fenetre_principale.destroy()
        ]
    
    # Create candidate cards
    for i, candidat in enumerate(candidats):
        # Card Frame
        card_frame = tk.Frame(
            scrollable_frame,
            bg=COLORS['card_bg'],
            relief=tk.FLAT,
            borderwidth=0
        )
        card_frame.pack(pady=10, padx=40, fill=tk.X)
        
        # Inner padding frame
        inner_frame = tk.Frame(card_frame, bg=COLORS['card_bg'])
        inner_frame.pack(padx=20, pady=15, fill=tk.BOTH)
        
        # Candidate number and name
        name_frame = tk.Frame(inner_frame, bg=COLORS['card_bg'])
        name_frame.pack(fill=tk.X, pady=(0, 10))
        
        number_label = tk.Label(
            name_frame,
            text=f"#{i+1}",
            font=('Helvetica', 14, 'bold'),
            bg=COLORS['card_bg'],
            fg=COLORS['secondary']
        )
        number_label.pack(side=tk.LEFT, padx=(0, 10))
        
        name_label = tk.Label(
            name_frame,
            text=candidat,
            font=('Helvetica', 16, 'bold'),
            bg=COLORS['card_bg'],
            fg=COLORS['text_light'],
            anchor='w'
        )
        name_label.pack(side=tk.LEFT, fill=tk.X)
        
        # Vote button
        vote_btn = create_rounded_button(
            inner_frame,
            text="✓ VOTER",
            command=creer_action_vote(candidat),
            bg_color=COLORS['primary'],
            hover_color=COLORS['primary_hover'],
            width=15
        )
        vote_btn.pack(anchor=tk.E)
        
        # Hover effect for card
        def on_card_enter(e, frame=card_frame):
            frame['bg'] = COLORS['card_hover']
            for child in frame.winfo_children():
                if isinstance(child, tk.Frame):
                    child['bg'] = COLORS['card_hover']
                    for subchild in child.winfo_children():
                        if isinstance(subchild, (tk.Frame, tk.Label)):
                            subchild['bg'] = COLORS['card_hover']
        
        def on_card_leave(e, frame=card_frame):
            frame['bg'] = COLORS['card_bg']
            for child in frame.winfo_children():
                if isinstance(child, tk.Frame):
                    child['bg'] = COLORS['card_bg']
                    for subchild in child.winfo_children():
                        if isinstance(subchild, (tk.Frame, tk.Label)):
                            subchild['bg'] = COLORS['card_bg']
        
        card_frame.bind("<Enter>", on_card_enter)
        card_frame.bind("<Leave>", on_card_leave)
        for widget in [inner_frame, name_frame, number_label, name_label]:
            widget.bind("<Enter>", on_card_enter)
            widget.bind("<Leave>", on_card_leave)
    
    canvas.pack(side="left", fill="both", expand=True, padx=0, pady=0)
    scrollbar.pack(side="right", fill="y")
    
    # Footer with quit button
    footer_frame = tk.Frame(fenetre_principale, bg=COLORS['bg_dark'], height=80)
    footer_frame.pack(fill=tk.X, side=tk.BOTTOM)
    footer_frame.pack_propagate(False)
    
    bouton_quitter = create_rounded_button(
        footer_frame,
        text="❌ Quitter",
        command=fenetre_principale.destroy,
        bg_color=COLORS['error'],
        hover_color='#ff5252',
        width=20
    )
    bouton_quitter.pack(pady=15)

    fenetre_principale.mainloop()


def creer_fenetre_login():
    """Crée et affiche la fenêtre de login moderne."""
    
    login_fenetre = tk.Tk()
    login_fenetre.title("🔐 Connexion - Vote Électronique")
    login_fenetre.geometry("450x550")
    login_fenetre.configure(bg=COLORS['bg_dark'])
    login_fenetre.resizable(False, False)
    
    # Main container
    main_frame = tk.Frame(login_fenetre, bg=COLORS['bg_dark'])
    main_frame.pack(expand=True, fill=tk.BOTH, padx=40, pady=40)
    
    # Title Section
    title_frame = tk.Frame(main_frame, bg=COLORS['bg_dark'])
    title_frame.pack(pady=(0, 30))
    
    icon_label = tk.Label(
        title_frame,
        text="🔐",
        font=('Helvetica', 48),
        bg=COLORS['bg_dark']
    )
    icon_label.pack()
    
    title_label = tk.Label(
        title_frame,
        text="CONNEXION",
        font=('Helvetica', 28, 'bold'),
        bg=COLORS['bg_dark'],
        fg=COLORS['text_light']
    )
    title_label.pack(pady=(10, 5))
    
    subtitle_label = tk.Label(
        title_frame,
        text="Vote Électronique Sécurisé",
        font=('Helvetica', 11),
        bg=COLORS['bg_dark'],
        fg=COLORS['text_gray']
    )
    subtitle_label.pack()
    
    # Form Section
    form_frame = tk.Frame(main_frame, bg=COLORS['bg_dark'])
    form_frame.pack(pady=20, fill=tk.X)
    
    # Identifiant
    id_label = tk.Label(
        form_frame,
        text="Identifiant",
        font=('Helvetica', 11, 'bold'),
        bg=COLORS['bg_dark'],
        fg=COLORS['text_light'],
        anchor='w'
    )
    id_label.pack(fill=tk.X, pady=(10, 5))
    
    entry_id = create_styled_entry(form_frame)
    entry_id.pack(fill=tk.X, ipady=8)
    
    # Mot de passe
    mdp_label = tk.Label(
        form_frame,
        text="Mot de passe",
        font=('Helvetica', 11, 'bold'),
        bg=COLORS['bg_dark'],
        fg=COLORS['text_light'],
        anchor='w'
    )
    mdp_label.pack(fill=tk.X, pady=(20, 5))
    
    entry_mdp = create_styled_entry(form_frame, show="•")
    entry_mdp.pack(fill=tk.X, ipady=8)
    
    # Error label
    label_erreur = tk.Label(
        form_frame,
        text="",
        font=('Helvetica', 10, 'bold'),
        bg=COLORS['bg_dark'],
        fg=COLORS['error']
    )
    label_erreur.pack(pady=(15, 0))

    def update():
        global info
        info = client.read()
        if info != "E":
            func()
        login_fenetre.after(500, update)
    
    def verifier_login():
        global info
        global func
        identifiant = entry_id.get()
        mdp = entry_mdp.get()
        
        if not identifiant or not mdp:
            label_erreur.config(text="⚠️ Veuillez remplir tous les champs", fg=COLORS['warning'])
            return
        
        client.login(identifiant, mdp)
        
        def log():
            login = False
            code = info
            print(code)
            if code == "E":
                label_erreur.config(text="❌ Le serveur ne répond pas !", fg=COLORS['error'])
            elif code == "E2":
                label_erreur.config(text="❌ Identifiant invalide", fg=COLORS['error'])
            elif code == "E3":
                label_erreur.config(text="❌ Mot de passe invalide", fg=COLORS['error'])
            elif code == "E10":
                label_erreur.config(text="⚠️ Vote non ouvert", fg=COLORS['warning'])
            elif code == "O0":
                label_erreur.config(text="✓ Connexion réussie !", fg=COLORS['success'])
                login = True
            else:
                label_erreur.config(text="❌ Erreur inconnue, connexion fermée", fg=COLORS['error'])
                client.disconnect()
                login_fenetre.destroy()

            if login:
                print("Connexion réussie !")
                login_fenetre.destroy() 
                ouvrir_fenetre_principale()
        
        func = log
    
    # Bind Enter key to login
    def on_enter(event):
        verifier_login()
    
    entry_id.bind('<Return>', on_enter)
    entry_mdp.bind('<Return>', on_enter)
    
    # Connect button
    btn_connecter = create_rounded_button(
        form_frame,
        text="🔓 SE CONNECTER",
        command=verifier_login,
        bg_color=COLORS['primary'],
        hover_color=COLORS['primary_hover'],
        width=25
    )
    btn_connecter.pack(pady=(25, 10), fill=tk.X, ipady=5)
    
    # Footer info
    footer_label = tk.Label(
        main_frame,
        text="Système de vote électronique sécurisé",
        font=('Helvetica', 9),
        bg=COLORS['bg_dark'],
        fg=COLORS['text_gray']
    )
    footer_label.pack(side=tk.BOTTOM, pady=(20, 0))

    login_fenetre.after(100, update)
    login_fenetre.mainloop()


creer_fenetre_login()
