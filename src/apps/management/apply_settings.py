from trezor import ui, wire
from trezor.utils import unimport


@unimport
async def layout_apply_settings(session_id, msg):
    from trezor.messages.Success import Success
    from trezor.messages.FailureType import Other
    from trezor.ui.text import Text
    from ..common.confirm import require_confirm
    from ..common.request_pin import protect_by_pin
    from ..common import storage

    await protect_by_pin(session_id)

    if msg.homescreen is not None:
        raise wire.FailureError(
            Other, 'ApplySettings.homescreen is not supported')

    if msg.label is None and msg.language is None and msg.use_passphrase is None:
        raise wire.FailureError(Other, 'No setting provided')

    if msg.label is not None:
        await require_confirm(session_id, Text(
            'Change label', ui.ICON_RESET,
            'Do you really want to', 'change label to',
            ui.BOLD, '%s' % msg.label,
            ui.NORMAL, '?'))

    if msg.language is not None:
        await require_confirm(session_id, Text(
            'Change language', ui.ICON_RESET,
            'Do you really want to', 'change language to',
            ui.BOLD, '%s' % msg.language,
            ui.NORMAL, '?'))

    if msg.use_passphrase is not None:
        await require_confirm(session_id, Text(
            'Enable passphrase' if msg.use_passphrase else 'Disable passphrase',
            ui.ICON_RESET,
            'Do you really want to',
            'enable passphrase' if msg.use_passphrase else 'disable passphrase',
            'encryption?'))

    storage.load_settings(label=msg.label,
                          language=msg.language,
                          passphrase_protection=msg.use_passphrase)

    return Success(message='Settings applied')
