SELECT COUNT(*) 
FROM sqlite_master 
WHERE type='table' AND name IN ('Users', 'Accounts', 'Acoounts_Transfers', 'Transections');