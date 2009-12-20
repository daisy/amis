"""
Script for building the example.

Usage:
    python setup.py py2app
"""
from distutils.core import setup
import py2app

plist = dict(
    CFBundleDocumentTypes = [
        dict(
            CFBundleTypeExtensions=["html", "opf"],
            CFBundleTypeName="Daisy book",
            CFBundleTypeRole="Viewer",
            NSDocumentClass="AmisGuiDocument",
        ),
    ]
)


setup(
    app=["MacAmis.py"],
    data_files=["MainMenu.nib", "AmisGuiDocument.nib"],
    options=dict(py2app=dict(plist=plist)),
)
