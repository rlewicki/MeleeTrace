<h3 align="center">Melee Trace</h3>

# The What
This is a fairly generic plugin that enables you to setup melee traces directly in your animation assets.

# The Why
Within last few years I've been working on many prototypes that required functionality of melee weapon traces and every time I've been implementing this from scratch. I finally decided that it is a time to make it a plugin I can use myself in my prototypes as well as share it with community.

# The How
High level description would be that the hit detection is ran by sphere traces of pre-defined radius between previous frame skeletal mesh socket location and current frame skeletal mesh socket location. The skeletal mesh component can be either the character mesh itself or any other attached skeletal mesh e.g. weapon. For more detailed description and break down how it works please refer to the wiki page.

# Feature requests
Feel free to request features that you would like to be implemented as part of this plugin as GitHub issues.

# Gallery
## Melee tracing hitting a dummy target
<div style="width: 50%; float: left; padding-right:4px;">
<p>Debug drawing OFF</p>
    <video controls width="350">
        <source src="Gallery/TargetTraceNoDebug.mov" type="video/webm" />
    </video>
</div>
<div style="margin-left: 50%; padding-left:4px;">
<p>Debug drawing ON</p>
    <video controls width="350">
        <source src="Gallery/TargetTraceDebug.mov" type="video/webm" />
    </video>
</div>

## Misc attack animations
<video controls width="708">
    <source src="Gallery/MultipleAnimations.mov" type="video/webm" />
</video>

# Credits
Animations used in the gallery videos and sample projects are made by ZzGERTzZ, you can purchase them in on Marketplace [here](https://www.unrealengine.com/marketplace/en-US/product/close-combat-swordsman).