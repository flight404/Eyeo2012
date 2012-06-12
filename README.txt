If you are reading this, it means that you have decided to look at my code.
Code I banged out during a major life upheaval.
Code I wrote in a fit of hyperactivity.
Code I rushed to meet a conference deadline.
Bravo. You are very brave.

TECHNICAL CONSIDERATION
These projects were made with Cinder C++ framework which you can download at http://libcinder.org/download/. If you are completely new to C++, I would wholeheartedly recommend you turn around and just walk away. This is not the content you should cut your C++ chops with. Wait, is that even a proper turn of phrase? Seriously though, there are much more sane ports from which to embark. Really. Just go. 

However, if you do have some C++ experience, but maybe not experience with Cinder, be sure to read the Welcome.html which comes with the Cinder download.

If you are on a computer that isn't unibody aluminum with a fruit icon on it, I am sorry. I did not do a port to Visual Studio so you have a little work ahead of you. The code should generally work on PC but I have not tested it on anything other than this lovely MacBook Pro.

If you are a C++ genius with certificates to show for it, I apologize with unfettered intensity. Please don't send me angry emails. My heart can't take it.

WHERE TO BEGIN
I would recommend looking at the project called Room first. Also, each project has its own little readme file. Do what the filename says.

AGAIN, MY APOLOGIES
I am a bit of an oddity. I have never had any formal training in coding. I learned bits and pieces as I slowly made my way from Flash's ActionScript to Processing to Cinder. It has been a 14 year journey and today I still often feel as lost as the first time I looked at AS 1.0 code back in the days of Flash 4. I recognize that I can make some great looking work, and I am proud of this fact. But as soon as I am engaged in a code-related conversation with someone who knows C++, someone who knows proper code design, someone who knows how to explain the difference between a pointer and a reference, someone who polymorphs without hesitation, the bloom falls from the rose and I end up looking like an idiot. Or even worse, a fraud.

I have often expressed my coding ability using the following metaphor. Or is it a simile?

It is as if I were a surgeon. A surgeon that could point to dozens of currently living people and proudly exclaim, "were it not for me, these lovely people would be dead." However, I did not attend medical school. I never read any books on surgery, at least not all the way through. I just happen to live in a world with lax laws and zero accountability. This means I can cut into a person and remove the red beating thing and then notice that this makes them dead. Oh well. I toss the formerly beating thing into a bin, send the body down to the morgue, and move on to the next patient. What happens if I remove these two roundy things up top? Oh, you can't make pictures in your head anymore and everything is black? Oops, okay, noted. 

I do surgery by trial and error and there are thousands of bodies in my wake. Eventually, I learn that clumped grey-brown ickies should be removed, leaking red tube things should be patched, and loud screaming is usually a bad sign. After a while, I develop a reputation as someone who can get things done… someone that can open you up, fix the broken bits, and put you back together.

But as soon as a skilled surgeon wants to talk shop, I get confused. They say things like "Did you end up utilizing a transjugular intrahepatic portosystemic shunt?" Whereas I might say "I put in a tube thing that carries red life-juice out of the brown flat mass." Though I may look like I know what I am doing, and might even do some impressive things, I have a hard time having conversations about it because I just don't know the lingo. I don't know the history. I don't know the correct procedures. So for this, I am sorry.

The code in these projects is reasonable but certainly can and should be improved. I did some things well but I also did some things very poorly. Sometimes it was because I was rushing to implement a new feature. Sometimes it was because I changed my mind but didn't go back to change all the relevant bits. Sometimes it was because I didn't do the research and just didn't know any better.

There will be unused variables. There will be redundant methods. There might even be app crashing bugs that I never experienced by chance alone.

EYEO 2012 DISCLAIMERS
This code differs from what I used for my presentation. The talk I gave was carefully constructed to follow a specific arc. This means I shoehorned some functionality into the code that really shouldn't be there. The best example of this was from the project Catalog which eventually transitioned from a star guide to a view of the surface of Gliese 581g. This planet simulation was crammed in during the eleventh hour and it was very very very sloppy. I am surprised I didn't break the project. I was rushed, I didn't plan it out, and it was so cobbled together it would have made reading the code ten times more frustrating. So I have removed it.

Same with the Anti-Food particle which explodes when touching a Food particle in the Ant Mill project. I have removed that functionality because it has a home already. If you want to see how I did the explosion of balloons and confetti, that is covered in the project called Big Bang.

Also, I have taken out the audio. I am sorry to do this, but it is a combination of it requiring an outside library called FMOD and also using some sound effects that I had rights to play but possibly not the rights to distribute. If you are interested in audio integration, I recommend looking into FMOD and grabbing the CinderBlock by Roger Pala.
http://forum.libcinder.org/topic/fmod-ex-cinder-block-for-3d-sound-and-more

CONCLUSION
I hope you enjoy this code. Do with it what you will. I don't require any attribution or credit but would be happy to see what you end up doing with it. I realize that releasing code like this might invite those that lack imagination to just change a color or tweak a parameter and release it as their own. I can't do anything about this nor will I track you down. I just hope you take the time to put some of your own personality and experiences into it. Make it your own and we both win.

If you have trouble running these projects or you can't get Cinder to compile or you are getting linker errors or you are experiencing kernel panics or the frame rate is very slow, please do not email me. I am offering this code as is. It works fine on my Macbook Pro laptop and that is the only computer I have tested it on. If your problems are Cinder specific, the forum (https://forum.libcinder.org/) is a great place to find answers. If your issues are Xcode related, you have my sympathies.

Thank you for your interest.
Robert Hodgin
http://roberthodgin.com/


