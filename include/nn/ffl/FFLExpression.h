#ifndef FFL_EXPRESSION_H_
#define FFL_EXPRESSION_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLExpression
{
    // These are derived from nwf.mii.MiiExpression
    // NWF had duplicates, although I've marked ones
    // I have seen in other places (nn::mii, cdn-mii API)
    // as primary, meaning the other one is meant to be a duplicate.
    // The other names are usually seen in mii-secure (NNAS) CDN URLs.
    FFL_EXPRESSION_NORMAL                = 0,
    FFL_EXPRESSION_SMILE                 = 1,
    FFL_EXPRESSION_ANGER                 = 2,
    FFL_EXPRESSION_SORROW                = 3,   // primary
    FFL_EXPRESSION_PUZZLED               = 3,
    FFL_EXPRESSION_SURPRISE              = 4,   // primary
    FFL_EXPRESSION_SURPRISED             = 4,
    FFL_EXPRESSION_BLINK                 = 5,
    FFL_EXPRESSION_OPEN_MOUTH            = 6,
    FFL_EXPRESSION_SMILE_OPEN_MOUTH      = 7,   // primary
    FFL_EXPRESSION_HAPPY                 = 7,
    FFL_EXPRESSION_ANGER_OPEN_MOUTH      = 8,
    FFL_EXPRESSION_SORROW_OPEN_MOUTH     = 9,
    FFL_EXPRESSION_SURPRISE_OPEN_MOUTH   = 10,
    FFL_EXPRESSION_BLINK_OPEN_MOUTH      = 11,
    FFL_EXPRESSION_WINK_LEFT             = 12,
    FFL_EXPRESSION_WINK_RIGHT            = 13,
    FFL_EXPRESSION_WINK_LEFT_OPEN_MOUTH  = 14,
    FFL_EXPRESSION_WINK_RIGHT_OPEN_MOUTH = 15,
    FFL_EXPRESSION_LIKE_WINK_LEFT        = 16,  // primary
    FFL_EXPRESSION_LIKE                  = 16,
    FFL_EXPRESSION_LIKE_WINK_RIGHT       = 17,
    FFL_EXPRESSION_FRUSTRATED            = 18,


    // Below are from AFL/Miitomo.
    // I could not find internal names for any of the
    // expressions so I'm commenting the English name
    // although the jaJP ones may be more accurate? Not sure.
    FFL_EXPRESSION_19  = 19,  // Bored
    FFL_EXPRESSION_20  = 20,  // Bored open mouth
    FFL_EXPRESSION_21  = 21,  // Sigh mouth straight
    FFL_EXPRESSION_22  = 22,  // Sigh
    FFL_EXPRESSION_23  = 23,  // Disgusted mouth straight
    FFL_EXPRESSION_24  = 24,  // Disgusted
    FFL_EXPRESSION_25  = 25,  // Love
    FFL_EXPRESSION_26  = 26,  // Love mouth open
    FFL_EXPRESSION_27  = 27,  // Determined mouth straight
    FFL_EXPRESSION_28  = 28,  // Determined
    FFL_EXPRESSION_29  = 29,  // Cry mouth straight
    FFL_EXPRESSION_30  = 30,  // Cry
    FFL_EXPRESSION_31  = 31,  // Big smile mouth straight
                              // (same as 1/Smile? but mouth straight)
    FFL_EXPRESSION_32  = 32,  // Big smile
    FFL_EXPRESSION_33  = 33,  // Cheeky
    FFL_EXPRESSION_34  = 34,  // Cheeky duplicate
    FFL_EXPRESSION_35  = 35,  // ?? JoJo eyes? funny mouth
    FFL_EXPRESSION_36  = 36,  // ?? JoJo eyes? funny mouth mouth open
    FFL_EXPRESSION_37  = 37,  // Smug
    FFL_EXPRESSION_38  = 38,  // Smug mouth open
    FFL_EXPRESSION_39  = 39,  // Resolve
    FFL_EXPRESSION_40  = 40,  // Resolve mouth open
    FFL_EXPRESSION_41  = 41,  // Unbelievable
    FFL_EXPRESSION_42  = 42,  // Unbelievable duplicate
    FFL_EXPRESSION_43  = 43,  // Cunning
    FFL_EXPRESSION_44  = 44,  // Cunning duplicate
    FFL_EXPRESSION_45  = 45,  // Raspberry
    FFL_EXPRESSION_46  = 46,  // Raspberry duplicate
    FFL_EXPRESSION_47  = 47,  // Innocent
    FFL_EXPRESSION_48  = 48,  // Innocent duplicate
    FFL_EXPRESSION_49  = 49,  // Cat
    FFL_EXPRESSION_50  = 50,  // Cat duplicate
    FFL_EXPRESSION_51  = 51,  // Dog
    FFL_EXPRESSION_52  = 52,  // Dog duplicate
    FFL_EXPRESSION_53  = 53,  // Tasty
    FFL_EXPRESSION_54  = 54,  // Tasty duplicate
    FFL_EXPRESSION_55  = 55,  // Money mouth straight
    FFL_EXPRESSION_56  = 56,  // Money
    FFL_EXPRESSION_57  = 57,  // Spiral mouth straight
    FFL_EXPRESSION_58  = 58,  // Confused
    FFL_EXPRESSION_59  = 59,  // Cheerful mouth straight
    FFL_EXPRESSION_60  = 60,  // Cheerful
    FFL_EXPRESSION_61  = 61,  // Normal duplicate, maybe Blank?
    FFL_EXPRESSION_62  = 62,  // Normal duplicate, maybe Blank? duplicate
    FFL_EXPRESSION_63  = 63,  // Grumble mouth straight
    FFL_EXPRESSION_64  = 64,  // Grumble
    FFL_EXPRESSION_65  = 65,  // Moved mouth straight
    FFL_EXPRESSION_66  = 66,  // Moved
    FFL_EXPRESSION_67  = 67,  // Singing mouth small
    FFL_EXPRESSION_68  = 68,  // Singing
    FFL_EXPRESSION_69  = 69,  // Stunned

    FFL_EXPRESSION_MAX = 70
}
FFLExpression;

/* TODO: FIX THESE FLAWS

 * Apply eyebrow colors
   Unknown what these are supposed to be, have seen it dark and light.
FFL_EXPRESSION_39 (Resolve)
FFL_EXPRESSION_40 (Resolve mouth open)
FFL_EXPRESSION_49 (Cat)
FFL_EXPRESSION_50 (Cat duplicate)
FFL_EXPRESSION_51 (Dog)
FFL_EXPRESSION_52 (Dog duplicate)
 * Eyes are the wrong direction?:
   The eyes may be both in the wrong spot, AND reversed.
   NOTE: This is probably not going to be easy to fix, and
   it may be the reason why Mii Studio wink directions are wrong??
FFL_EXPRESSION_33 (Cheeky)
FFL_EXPRESSION_34 (Cheeky duplicate)
FFL_EXPRESSION_47 (Innocent)
FFL_EXPRESSION_48 (Innocent duplicate)
FFL_EXPRESSION_58 (Confused)
FFL_EXPRESSION_60 (Cheerful)
FFL_EXPRESSION_65 (Moved mouth straight)
FFL_EXPRESSION_66 (Moved)

*/

#ifdef __cplusplus
}
#endif

#endif // FFL_EXPRESSION_H_
