import sys
import MySQLdb
import cherrypy
from htmltemplates import *
from util import *

class Login:
    """Things relating to logging in"""
    def index(self):
        """show the login form"""
        return login_form()
    
    def process_login(self, username, password):
        if login(username, password) == None:
            return login_error()
        else:
            menu = MainMenu()
            return menu.index()
    
    index.exposed = True
    process_login.exposed = True


class MainMenu():
    def index(self):
        """Show the links for the main menu"""
        return main_menu_links(get_user())
    
    index.exposed = True

class TranslateLabels():
    def index(self):
        """Show the big table of translate-able items"""
        return "BIG TABLE OF TRANSLATE-ABLE ITEMS"
    
    index.exposed = True

#set up cherrypy
root = Login()
root.MainMenu = MainMenu()
root.TranslateLabels = TranslateLabels()
#root.show_mnemonics_page = show_mnemonics_page()
#root.show_accelerators_page = show_accelerators_page()

app = cherrypy.tree.mount(root, script_name='/')

if __name__ == '__main__':
    import os.path
    cherrypy.config.update(os.path.join(os.path.dirname(__file__), 'cherrypy.conf'))
    cherrypy.server.quickstart()
    cherrypy.engine.start()
