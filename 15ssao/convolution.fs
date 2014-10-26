/*
This 1D convolution filter is used in the two-dimensional Gaussian (blur) filter. It works like this. Consider this array of 10 pixels
     ___________________
    |o_o_o_o_o_o_o_o_o_o|
     0 1 2 3 4 5 6 7 8 9

The convolution is a sum of weighted neighbor samples around each pixel. The weighting is determined by the filter type, and the number of neighbor samples is determined by the length of the filter - also known as the kernel size.

For example, consider a box filter of length 3 on each side, where each neighbor is weighted by the same amount. The resulting convolution value at pixel 5 is then the average of pixel values from 5-3 to 5+3. 

Letting y(n) be the result and x(n) the original pixel array, we can express this in maths as

    y(5) = (1 / 7) (x(2) + x(3) + x(4) + x(5) + x(6) + x(7) + x(8))

The Gauss filter uses weights shaped like a bell function around the center pixel,

    w(n) = (1/sqrt(2pi sigma^2)) exp(-x^2 / (2sigma^2))

where sigma^2 is known as the variance. For example, a Gauss filter of length 3 on each side, with sigma = 2.0, has the following weight values

      -3   -2    -1     0     +1    +2    +3
    0.065 0.121 0.176 0.199 0.176 0.121 0.065

Naturally, the filter is symmetric. Note that in the case of the box filter above, the sum of weights (1/7) was equal to 1. This is important, otherwise the filtered pixels would appear darker or brighter than the original pixels.

But in the case of the Gauss filter, the weights above do _not_ add to 1. This is because the theoretically correct filter is of infinite length, while we have truncated it to a finite length.

For instance, the sum of weights above is 0.923. We could amend this by simply adding the remainder (0.077) to the centre value. Mathematically, applying the Gauss filter looks like this

             3
    y(5) =  SUM w(k) x(5 - k)
            -3

where w(k) are the weights above.
*/