Add your crew's remote repository, and call it upstream
git remote add upstream https://github.com/UCDBioe3090Blue/Bioprinter.git

Add the Bioe repository, call it headwaters (really, you can call it anything you want)
git remote add headwaters https://UCDBioe3090Blue@github.com/UCDBioe/Bioprinter.git

Pull the headwaters remote into your local repo
git pull headwaters master
git fetch headwaters master
git diff master headwaters/master
git diff --name-only master headwaters/master
git merge headwaters/master 

Pull the upstream remote into your local repo
git pull upstream master

Push updated local repo to origin
git push origin master

