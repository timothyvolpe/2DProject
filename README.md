<h1>2DProject</h1>

<p>Also known as "name not yet decided." My take on a 2D game engine coded mostly from scratch with the help of a few "basic" libraries. The only goal so far is a functional 2D world with procedurally generated, possibly infinite terrain, and support for Lua extendability.</p>

<h3>Installing</h3>

Installation is done through the use of CMake. The appropriate paths must be supplied for the required libraries (see <a href="https://github.com/timothyvolpe/2DProject#built-with">Built With</a>). Additionally the debug working directory must be set correctly in the generate project files. So far it has only been tested Visual Studio Community 2015.

Currently the game will only run on Windows. 

<h3>Built With</h3>

<ul>
	<li><a href="https://www.boost.org/">Boost</a> 1.59.0 - Uses a variety of Boost libraries</li>
	<li><a href="https://www.libsdl.org/index.php">SDL</a> 2.0.3 - Handling of OpenGL context and game window, input events, etc.</li>
	<li><a href="http://glew.sourceforge.net/">GLEW</a> 1.2 - Handling of OpenGL extensions</li>
	<li><a href="https://glm.g-truc.net/0.9.9/index.html">GLM</a> 0.9.3.2 - 3D math library</li>
	<li><a href="http://www.libpng.org/pub/png/libpng.html">libpng</a> 1.6.21 - Handling of PNG image files</li>
	<li><a href="https://box2d.org/">Box2D</a> 2.3.0 - Handling of game physics</li>
	<li><a href="https://www.freetype.org/">FreeType</a> 2.5.3 - Handling of font loading and rendering</li>
	<li><a href="https://www.lua.org/">Lua</a> 5.3.2 - Handling of extendability through scripts</li>
</ul>

<h3>Authors</h3>

<ul>
	<li>Timothy Volpe (<a href="mailto:timothybvolpe@gmail.com">timothybvolpe@gmail.com</a>) - <i>Engine Programming, Artwork, Lua</i></li>
</ul>

<h3>Licensing</h3>

<p>Copyright 2018-2019 Timothy Volpe (<a href="mailto:timothybvolpe@gmail.com">timothybvolpe@gmail.com</a>)<br/>All rights reserved.</p>

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

<h3>Acknowledgements</h3>
<ul>
	<li>
		<strong>edtaa3:</strong>
		<p>Copyright 2009 Stefan Gustavson (<a href="mailto:stefan.gustavson@gmail.com">stefan.gustavson@gmail.com</a>)<br/>All rights reserved.</p>
	</li>
	<li>
		<strong>distance-field:</strong>
		<a href="https://github.com/rougier/freetype-gl">freetype-gl</a>
		<p>Copyright 2011-2016 Nicolas P. Rougier<br/>Copyright 2013-2016 Marcel Metz<br/>All rights reserved.</p>
	</li>
</ul>
