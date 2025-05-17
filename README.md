<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
</head>

<body>

# 2024-2025-4GP-JoakimGomes-LucieSvc-Poupouj
## Développement d'un capteur low-tech à base de Graphite : de la conception au test

### Sommaire
<hr/>
<ul>
<li> <a href="#Introduction">Introduction </a></li>
<li> <a href="#Livrables">Livrables </a></li>
<li> <a href="#Matériel à disposition">Matériel à disposition </a></li>
<li> <a href="#Simulation électronique sur LTSpice">Simulation électronique sur LTSpice </a></li>
<li> <a href="#Design du PCB avec KiCad">Design du PCB avec KiCad </a></li>
<li> <a href="#Développement du code Arduino">Développement du code Arduino </a></li>
<li> <a href="#Développement de l'application mobile APK avec MIT App Inventor">Développement de l'application mobile APK avec MIT App Inventor </a></li>
<li> <a href="#Banc de test">Banc de test </a></li>
<li> <a href="#Datasheet">Datasheet </a></li>
<li> <a href="#Conclusion">Conclusion </a></li>
</ul>

<h2 id="Introduction">Introduction</h2>

<p>L'UF "du capteur au banc de test" a pour objectif de concevoir un capteur low-tech et de pouvoir ensuite le tester afin de le comparer avec un capteur commercial. Le capteur est à base de graphite, que l'on retrouve notamment dans nos crayons à papier. Son développement est issu de l'article scientifique <i>"Pencil Drawn Strain Gauges and Chemiresistors on Paper"</i>(Cheng-Wei Lin*, Zhibo Zhao*, Jaemyung Kim & Jiaxing Huang). Concevoir ce capteur est relativement simple : on applique à l'aide d'un crayon, du graphite sur un morceau de papier. Les couches de graphite déposées sur le papier agissent comme un conducteur électrique qui, lorsqu'on déforme le papier, a une conductivité (et donc une résistance) qui varie. Cela s'explique par le fait que  le nombre de chaînes de particules de graphite connectées varie selon le type de déformation. En tension, il y a moins de chaines et ces dernières sont plus courtes. C'est le contraire en compression avec des chaines plus proches, laissant ainsi mieux passer le courant.</p>

![Fonctionnement du capteur](images/capteur_fonctionnement.jpg)
<i>Figure 1 : Image issue de l'article "Pencil Drawn Strain Gauges and Chemiresistors on Paper" qui explique les déformations des chaines sous compression et tension</i>

<h2 id="Livrables">Livrables</h2>
<p>Les livrables de ce projet sont les suivants:</p>
<ul>
<li>Un schield PCB connecté à une carte Arduino UNO et contenant divers éléments:
<ul>
<li>Notre capteur de contrainte Low-tech</li>
<li>Un capteur de flexion commercial</li>
<li>Un amplificateur dont on pourra modifier le gain grâce à un potentiomètre pour le traitement du signal</li>
<li>Un écran OLED pour afficher les données du capteur</li>
<li>Un module Bluetooth permettant la communication avec un téléphone portable</li>
<li>Un encodeur rotatoire pour naviguer dans le menu de l'écran OLED</li>

</ul>
</li>
<li>
Le code Arduino qui permet de faire fonctionner tout le système : la communication Bluetooth, la gestion de l'écran OLED avec affichage des données, l'utilisation des deux capteurs de contrainte, la navigation dans le menu de l'écran OLED grâce à l'encodeur rotatoire, la modification du gain grâce au potentiomètre digital.
</li>
<li>
L'application Android developpée sur MIT App Inventor, permettant de visualiser les données directement sur son téléphone grâce à la communication Bluetooth.
</li>
<li>
Une datasheet pour le capteur en graphite détaillant ses performances et caractéristiques techniques.
</li>
</ul>

<h2 id="Matériel à disposition">Matériel à disposition</h2>
<p>Voici la liste du matériel nécessaire pour concevoir le système ci-dessus:</p>

<h2 id="Simulation électronique sur LTSpice">Simulation électronique sur LTSpice</h2>
<p>Contenu de Simulation électronique sur LTSpice...</p>

<h2 id="Design du PCB avec KiCad">Design du PCB avec KiCad</h2>
<p>Contenu de Design du PCB avec KiCad...</p>

<h2 id="Développement du code Arduino">Développement du code Arduino</h2>
<p>Contenu de Développement du code Arduino...</p>

<h2 id="Développement de l'application mobile APK avec MIT App Inventor">Développement de l'application mobile APK avec MIT App Inventor</h2>
<p>Contenu de Développement de l'application mobile APK avec MIT App Inventor...</p>

<h2 id="Banc de test">Banc de test</h2>
<p>Contenu de Banc de test...</p>

<h2 id="Datasheet">Datasheet</h2>
<p>Contenu de Datasheet...</p>

<h2 id="Conclusion">Conclusion</h2>
<p>Contenu de Conclusion...</p>
</body>
</html>